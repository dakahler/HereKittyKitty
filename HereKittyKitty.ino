#include "LiquidCrystalEx.h"
#include "EasyDriver.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <Time.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;
const int compartmentsPerRevolution = 6;
const int stepsPerCompartment = stepsPerRevolution / compartmentsPerRevolution;
const int ouncesPerCompartment = 1;

const int feedOunces = 4; // one compartment per ounce
const int feedSteps = (feedOunces / ouncesPerCompartment) * stepsPerCompartment;

const int feedNumDaily = 5; // number of feedings in a day
const int feedStartTime = 6; // hour
const int feedSpeed = 5;


static RTC_DS1307 s_rtc;
static LiquidCrystalEx s_lcd(7, 8, 9, 10, 11 , 12);
static EasyDriver s_easyDriver(2, 3, 4, 5, 6);

static uint8_t feedHours[12];
static int s_currentFeedIndex = 0;
static int s_previousLoopHour = 0;

static Timer s_updateLcdTimer(1000, []()
{
	DateTime now = s_rtc.now();

	char timeString[18];
	sprintf(timeString, "%02d:%02d:%02d %s",
		hourFormat12(now.hour()), now.minute(), now.second(), isAM(now.hour()) ? "AM" : "PM");

	s_lcd.Print(0, 0, timeString);
	s_lcd.Print(13, 0, feedOunces);
	s_lcd.Print(15, 0, feedNumDaily);

	sprintf(timeString, "Next: %02d:00 %s", hourFormat12(feedHours[s_currentFeedIndex]),
		isAM(feedHours[s_currentFeedIndex]) ? "AM" : "PM");

	s_lcd.Print(0, 1, timeString);
});

static Timer s_motorFeedTimer(feedSpeed, []()
{
	s_easyDriver.Step();
}, feedSteps * 8, false,

[]() // on start
{
	s_easyDriver.EnableMotor();
	s_easyDriver.SetDirection(true);
},

[]() // on stop
{
	s_easyDriver.DisableMotor();
});

static ButtonPress s_feedNowButton(26, []()
{
	feedNow();
});

static IUpdatable* s_updatables[] { &s_updateLcdTimer, &s_motorFeedTimer, &s_feedNowButton };

static void feedNow()
{
	s_motorFeedTimer.Restart();
}

void setup()
{ 
	Serial.begin(9600);
	Wire.begin();
	s_rtc.begin();
	if (!s_rtc.isrunning())
	{
		// TODO: Need better way of syncing time
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		s_rtc.adjust(DateTime(__DATE__, __TIME__));
	}

	int feedInterval = 24 / feedNumDaily;
	bool setCurrentFeedIndex = false;
	for (int i = 0; i < feedNumDaily; i++)
	{
		feedHours[i] = feedStartTime + (feedInterval * i);

		if (!setCurrentFeedIndex)
		{
			if (feedHours[i] >= s_rtc.now().hour())
			{
				setCurrentFeedIndex = true;
				s_currentFeedIndex = i;
			}
		}
	}

	s_previousLoopHour = s_rtc.now().hour();

	s_lcd.begin(16, 2);
	s_updateLcdTimer.FireNow();
}

void loop()
{
	DateTime now = s_rtc.now();
	if (now.hour() != s_previousLoopHour)
	{
		s_previousLoopHour = now.hour();
		if (feedHours[s_currentFeedIndex] == now.hour())
		{
			// Food!
			feedNow();
			s_currentFeedIndex++;
			if (s_currentFeedIndex >= feedNumDaily)
			{
				s_currentFeedIndex = 0;
			}
		}
	}

	for (IUpdatable* u : s_updatables)
	{
		u->Update();
	}
}
