// Program.h

#ifndef _PROGRAM_h
#define _PROGRAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"
#include <RTClib.h>
#include "LiquidCrystalEx.h"
#include "EasyDriver.h"
#include "Timer.h"
#include "ButtonPress.h"

class Program : public IUpdatable
{
public:
	static Program* GetInstance();
	void Update() override;

private:
	Program();

	static Program* m_instance;

	static void FeedNow();
	static void UpdateLcd();
	static void StartMotor();
	static void StopMotor();
	static void StepMotor();

	static const int stepsPerRevolution = 200;
	static const int compartmentsPerRevolution = 6;
	static const int stepsPerCompartment = stepsPerRevolution / compartmentsPerRevolution;
	static const int ouncesPerCompartment = 1;

	static const int feedOunces = 4; // one compartment per ounce
	static const int feedSteps = (feedOunces / ouncesPerCompartment) * stepsPerCompartment;

	static const int feedNumDaily = 5; // number of feedings in a day
	static const int feedStartTime = 6; // hour
	static const int feedSpeed = 5;

	static RTC_DS1307 s_rtc;
	static LiquidCrystalEx s_lcd;
	static EasyDriver s_easyDriver;

	static uint8_t s_feedHours[12];
	static int s_currentFeedIndex;
	static int s_previousLoopHour;

	static Timer s_updateLcdTimer;
	static Timer s_motorFeedTimer;
	static ButtonPress s_feedNowButton;
};

#endif

