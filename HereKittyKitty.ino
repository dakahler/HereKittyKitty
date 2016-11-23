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

uint8_t* feedHours;
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
	// Enable motor
	digitalWrite(EN, LOW);

	digitalWrite(dir, LOW); // Pull direction pin low to move "forward"

	// Have to multiply steps by 8 because we're in high-resolution mode
	for(int x = 0; x < feedSteps * 8; x++)  // Loop the forward stepping enough times for motion to be visible
	{
		digitalWrite(stp, HIGH); // Trigger one step forward
		delay(feedSpeed);
		digitalWrite(stp, LOW); // Pull step pin low so it can be triggered again
		delay(feedSpeed);
	}

	// Disable motor
	digitalWrite(EN, HIGH);
}

void setup()
{ 
	Serial.begin(9600);
	Wire.begin();
	RTC.begin();
	if (!RTC.isrunning())
	{
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

	lcd.begin(16, 2);

	int feedInterval = 24 / feedNumDaily;
	feedHours = new uint8_t[feedNumDaily];
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
}

bool test = false;

void loop () {
    DateTime now = RTC.now(); 
	lcd.setCursor(0,0);

	if (!test)	
	{
		test = true;
		feedNow();
	}

	// Wow Arduino string concat is painful
	timeString = String();
	padDigit(hourFormat12(now.hour()), timeString);
	timeString += ":";
	padDigit(now.minute(), timeString);
	timeString += ":";
	padDigit(now.second(), timeString);
	timeString += " ";
	timeString += isAM(now.hour()) ? "AM" : "PM";

	lcd.print(timeString);

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

	lcd.setCursor(13, 0);
	lcd.print(feedOunces);

	lcd.setCursor(15, 0);
	lcd.print(feedNumDaily);

	String nextFeedString = String();
	nextFeedString += "Next: ";
	padDigit(hourFormat12(feedHours[currentFeedIndex]), nextFeedString);
	nextFeedString += ":00 ";
	nextFeedString += isAM(feedHours[currentFeedIndex]) ? "AM" : "PM";

	lcd.setCursor(0, 1);
	lcd.print(nextFeedString);

    delay(1000);
}
