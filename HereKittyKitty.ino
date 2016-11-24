#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <Time.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Stepper.h>

#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6

RTC_DS1307 RTC;
String timeString;
LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

uint8_t feedHours[12];
int currentFeedIndex = 0;
int previousLoopHour = 0;

const int stepsPerRevolution = 200;
const int compartmentsPerRevolution = 6;
const int stepsPerCompartment = stepsPerRevolution / compartmentsPerRevolution;
const int ouncesPerCompartment = 1;

const int feedOunces = 4; // one compartment per ounce
const int feedSteps = (feedOunces / ouncesPerCompartment) * stepsPerCompartment;

const int feedNumDaily = 5; // number of feedings in a day
const int feedStartTime = 6; // hour
const int feedSpeed = 5;

Timer g_updateLcdTimer(1000, []()
{
	DateTime now = RTC.now();
	lcd.setCursor(0, 0);

	char timeString[18];
	sprintf(timeString, "%02d:%02d:%02d %s",
		hourFormat12(now.hour()), now.minute(), now.second(), isAM(now.hour()) ? "AM" : "PM");

	lcd.print(timeString);

	lcd.setCursor(13, 0);
	lcd.print(feedOunces);

	lcd.setCursor(15, 0);
	lcd.print(feedNumDaily);

	sprintf(timeString, "Next: %02d:%02d %s", hourFormat12(feedHours[currentFeedIndex]),
		isAM(feedHours[currentFeedIndex]) ? "AM" : "PM");

	lcd.setCursor(0, 1);
	lcd.print(timeString);
});

Timer g_motorFeedTimer(feedSpeed, []()
{
	static bool isHigh = true;
	digitalWrite(stp, isHigh ? HIGH : LOW);
	isHigh = !isHigh;

}, feedSteps * 8, false,

[]() // on start
{
	// Enable motor
	digitalWrite(EN, LOW);

	// Pull direction pin low to move "forward"
	digitalWrite(dir, LOW);
},

[]() // on stop
{
	// Disable motor
	digitalWrite(EN, HIGH);
});

ButtonPress g_feedNowButton(16, []()
{

});

IUpdatable* g_updatables[] { &g_updateLcdTimer, &g_motorFeedTimer, &g_feedNowButton };

//Reset Easy Driver pins to default states
void resetEDPins()
{
	digitalWrite(stp, LOW);
	digitalWrite(dir, LOW);
	digitalWrite(MS1, LOW);
	digitalWrite(MS2, LOW);
	digitalWrite(EN, HIGH);
}

void feedNow()
{
	g_motorFeedTimer.Restart();
}

void setup()
{ 
	Serial.begin(9600);
	Wire.begin();
	RTC.begin();
	if (!RTC.isrunning())
	{
		// TODO: Need better way of syncing time
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}

	pinMode(stp, OUTPUT);
	pinMode(dir, OUTPUT);
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
	pinMode(EN, OUTPUT);
	resetEDPins(); //Set step, direction, microstep and enable pins to default states

	int feedInterval = 24 / feedNumDaily;
	bool setCurrentFeedIndex = false;
	for (int i = 0; i < feedNumDaily; i++)
	{
		feedHours[i] = feedStartTime + (feedInterval * i);

		if (!setCurrentFeedIndex)
		{
			if (feedHours[i] >= RTC.now().hour())
			{
				setCurrentFeedIndex = true;
				currentFeedIndex = i;
			}
		}
	}

	previousLoopHour = RTC.now().hour();

	digitalWrite(MS1, HIGH); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
	digitalWrite(MS2, HIGH);

	lcd.begin(16, 2);
	g_updateLcdTimer.FireNow();
}

void loop()
{
	DateTime now = RTC.now();
	if (now.hour() != previousLoopHour)
	{
		previousLoopHour = now.hour();
		if (feedHours[currentFeedIndex] == now.hour())
		{
			// Food!
			feedNow();
			currentFeedIndex++;
			if (currentFeedIndex >= feedNumDaily)
			{
				currentFeedIndex = 0;
			}
		}
	}

	for (IUpdatable* u : g_updatables)
	{
		u->Update();
	}
}
