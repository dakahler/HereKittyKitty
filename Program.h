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
#include "AudioPlayer.h"

class Program : public IUpdatable
{
public:
	static Program* GetInstance();
	void Update() override;

private:
	Program();

	static Program* m_instance;

	static void DoAction();
	static void UpdateLcd();
	static void StartMotor();
	static void StopMotor();
	static void StepMotor();
	static void ExitSettings();
	static void ChangePage();

	void RecalculateMealTimes();

	const int c_stepsPerRevolution = 200;
	const int c_compartmentsPerRevolution = 6;
	const int c_stepsPerCompartment = c_stepsPerRevolution / c_compartmentsPerRevolution;
	const int c_ouncesPerCompartment = 1;

	const unsigned long c_feedSpeed = 25;
	const int c_maxOuncesPerMeal = 8;
	const int c_maxMealsPerDay = 8;

	RTC_DS1307 m_rtc;
	LiquidCrystalEx m_lcd;
	EasyDriver m_easyDriver;

	uint8_t m_feedHours[12];
	int m_currentFeedIndex;
	int m_previousLoopHour;

	Timer m_updateLcdTimer;
	Timer m_motorFeedTimer;
	Timer m_exitSettingsTimer;
	ButtonPress m_actionButton;
	ButtonPress m_changePageButton;
	AudioPlayer m_audioPlayer;

	enum Page
	{
		Main,
		OuncesPerMeal,
		MealsPerDay,
		StartHour,
		NumPages
	} m_currentPage;

	struct EepromData
	{
		short Version = 0x0101;
		byte OuncesPerMeal = 4;
		byte MealsPerDay = 5;
		byte StartHour = 6;
	} m_eepromData;
};

#endif
