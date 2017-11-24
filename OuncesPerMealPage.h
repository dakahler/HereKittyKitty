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
	void WriteToEepRom(Preferences& preferences) const override;
	void ReadFromEepRom(Preferences& preferences) override;
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

