// MainPage.h

#ifndef _MAINPAGE_h
#define _MAINPAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Page.h"
#include "Timer.h"
#include "EasyDriver.h"
#include "AudioPlayer.h"

class MainPage : public IPage
{

public:
	MainPage(byte& ouncesPerMeal);

	void Update() override;
	int WriteToEepRom(int offset) const override;
	int ReadFromEepRom(int offset) const override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

private:
	Timer m_motorFeedTimer;
	byte& m_ouncesPerMeal;
	static bool isFeeding;

	static const int c_stepsPerRevolution = 200;
	static const int c_compartmentsPerRevolution = 6;
	static const int c_stepsPerCompartment = c_stepsPerRevolution / c_compartmentsPerRevolution;
	static const int c_ouncesPerCompartment = 1;
	static const unsigned long c_feedSpeed = 25;

	static EasyDriver s_easyDriver;
	static AudioPlayer s_audioPlayer;
	static void StartMotor();
	static void StopMotor();
	static void StepMotor();
};

#endif

