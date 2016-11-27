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
	int WriteToEepRom(int offset) override;
	int ReadFromEepRom(int offset) override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

private:
	Timer m_motorFeedTimer;
	byte& m_ouncesPerMeal;

	const int c_stepsPerRevolution = 200;
	const int c_compartmentsPerRevolution = 6;
	const int c_stepsPerCompartment = c_stepsPerRevolution / c_compartmentsPerRevolution;
	const int c_ouncesPerCompartment = 1;
	const unsigned long c_feedSpeed = 25;

	static EasyDriver s_easyDriver;
	static AudioPlayer s_audioPlayer;
	static void StartMotor();
	static void StopMotor();
	static void StepMotor();
};

#endif

