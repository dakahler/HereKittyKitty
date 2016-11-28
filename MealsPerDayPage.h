#ifndef _MEALSPERDAYPAGE_h
#define _MEALSPERDAYPAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Page.h"

class MealsPerDayPage : public IPage
{

public:
	MealsPerDayPage();

	void Update() override;
	int WriteToEepRom(int offset) const override;
	int ReadFromEepRom(int offset) const override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

	byte GetMealsPerDay()
	{
		return m_mealsPerDay;
	}

private:
	byte m_mealsPerDay = 4;

	static const int c_maxMealsPerDay = 8;
};

#endif // _MEALSPERDAYPAGE_h

