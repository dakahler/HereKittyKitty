#ifndef _LIQUIDCRYSTALEX_h
#define _LIQUIDCRYSTALEX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SSD1306.h>

class LiquidCrystalEx : public SSD1306
{
public:
	LiquidCrystalEx(uint8_t _address, uint8_t _sda, uint8_t _scl, uint8_t rst);

	void Print(int x, int y, const char* str);
	void Print(int x, int y, int num);
};

#endif

