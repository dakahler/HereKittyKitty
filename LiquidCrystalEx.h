#ifndef _LIQUIDCRYSTALEX_h
#define _LIQUIDCRYSTALEX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <LiquidCrystal.h>

class LiquidCrystalEx : public LiquidCrystal
{
public:
	LiquidCrystalEx(uint8_t rs, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

	void Print(int x, int y, const char* str);
	void Print(int x, int y, int num);
};

#endif

