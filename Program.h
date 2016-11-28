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
#include "MainPage.h"
#include "OuncesPerMealPage.h"
#include "MealsPerDayPage.h"
#include "StartHourPage.h"

class Program : public IUpdatable
{
public:
	static Program* GetInstance();
	void Update() override;
	int GetNextMealHour() const;

private:
	Program();

	static Program* m_instance;

	void DoAction(const ButtonPress<Program>& button);
	void UpdateLcd(const Timer<Program>& timer);
	void ExitSettings(const Timer<Program>& timer);
	void ChangePage(const ButtonPress<Program>& button);

	void RecalculateMealTimes();
	void Save();
	void Load();

	RTC_DS1307 m_rtc;
	LiquidCrystalEx m_lcd;

	uint8_t m_feedHours[12];
	int m_currentFeedIndex;
	int m_previousLoopHour;

	Timer<Program> m_updateLcdTimer;
	Timer<Program> m_exitSettingsTimer;
	ButtonPress<Program> m_actionButton;
	ButtonPress<Program> m_changePageButton;

	OuncesPerMealPage* m_ouncesPerMealPage;
	MealsPerDayPage* m_mealsPerDayPage;
	StartHourPage* m_startHourPage;
	MainPage* m_mainPage;

	IPage* m_currentPage = NULL;

	short Version = 0x0102;
};

#endif
