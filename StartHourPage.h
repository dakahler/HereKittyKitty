#ifndef _STARTHOURPAGE_h
#define _STARTHOURPAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Page.h"

class StartHourPage : public IPage
{

public:
	StartHourPage();

	void Update() override;
	void WriteToEepRom(Preferences& preferences) const override;
	void ReadFromEepRom(Preferences& preferences) override;
	void InvokeAction() override;
	void UpdateLcd(LiquidCrystalEx& lcd) override;

	byte GetStartHour()
	{
		return m_startHour;
	}

private:
	byte m_startHour = 6;
};

#endif // _STARTHOURPAGE_h

