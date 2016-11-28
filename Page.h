#ifndef _PAGE_h
#define _PAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"

class LiquidCrystalEx;

class IPage : IUpdatable
{
public:
	virtual void Update() = 0;
	virtual int WriteToEepRom(int offset) const = 0;
	virtual int ReadFromEepRom(int offset) const = 0;
	virtual void InvokeAction() = 0;
	virtual void UpdateLcd(LiquidCrystalEx& lcd) = 0;
};

#endif

