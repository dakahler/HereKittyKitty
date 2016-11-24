#include "EasyDriver.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <Time.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Stepper.h>

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

EasyDriver g_easyDriver(2, 3, 4, 5, 6);

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

	sprintf(timeString, "Next: %02d:00 %s", hourFormat12(feedHours[currentFeedIndex]),
		isAM(feedHours[currentFeedIndex]) ? "AM" : "PM");

	lcd.setCursor(0, 1);
	lcd.print(timeString);
});

Timer g_motorFeedTimer(feedSpeed, []()
{
	g_easyDriver.Step();
}, feedSteps * 8, false,

[]() // on start
{
	g_easyDriver.EnableMotor();
	g_easyDriver.SetDirection(true);
},

[]() // on stop
{
	g_easyDriver.DisableMotor();
});

ButtonPress g_feedNowButton(26, []()
{
	feedNow();
});

IUpdatable* g_updatables[] { &g_updateLcdTimer, &g_motorFeedTimer, &g_feedNowButton };

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
