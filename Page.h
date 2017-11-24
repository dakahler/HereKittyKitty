#ifndef _PAGE_h
#define _PAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"

class LiquidCrystalEx;
class Preferences;

class IPage : IUpdatable
{
public:
	virtual void Update() = 0;
	virtual void WriteToEepRom(Preferences& preferences) const = 0;
	virtual void ReadFromEepRom(Preferences& preferences) = 0;
	virtual void InvokeAction() = 0;
	virtual void UpdateLcd(LiquidCrystalEx& lcd) = 0;
};

#endif

