#ifndef _OUNCESPERMEALPAGE_h
#define _OUNCESPERMEALPAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Page.h"

class OuncesPerMealPage : public IPage
{

public:
	OuncesPerMealPage();

	void Update() override;
	int WriteToEepRom(int offset) const override;
	int ReadFromEepRom(int offset) const override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

	byte GetOuncesPerMeal()
	{
		return m_ouncesPerMeal;
	}

private:
	byte m_ouncesPerMeal;

	static const int c_maxOuncesPerMeal = 8;
	friend class Program;
};

#endif // _OUNCESPERMEALPAGE_h

