#include "LiquidCrystalEx.h"

LiquidCrystalEx::LiquidCrystalEx(uint8_t rs, uint8_t enable,
	uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
	: LiquidCrystal(rs, enable, d0, d1, d2, d3)
{

}

void LiquidCrystalEx::Print(int x, int y, const char* str)
{
	this->setCursor(x, y);
	this->print(str);
}

void LiquidCrystalEx::Print(int x, int y, int num)
{
	this->setCursor(x, y);
	this->print(num);
}
