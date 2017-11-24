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
	void WriteToEepRom(Preferences& preferences) const override;
	void ReadFromEepRom(Preferences& preferences) override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

private:
	Timer<MainPage> m_motorFeedTimer;
	byte& m_ouncesPerMeal;
	bool m_isFeeding;

	static const int c_stepsPerRevolution = 200;
	static const int c_compartmentsPerRevolution = 6;
	static const int c_stepsPerCompartment = c_stepsPerRevolution / c_compartmentsPerRevolution;
	static const int c_ouncesPerCompartment = 1;
	static const unsigned long c_feedSpeed = 25;

	EasyDriver m_easyDriver;
	AudioPlayer m_audioPlayer;
	void StartMotor(const Timer<MainPage>& timer);
	void StopMotor(const Timer<MainPage>& timer);
	void StepMotor(const Timer<MainPage>& timer);
};

#endif

