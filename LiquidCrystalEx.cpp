#include "LiquidCrystalEx.h"

LiquidCrystalEx::LiquidCrystalEx(uint8_t _address, uint8_t _sda, uint8_t _scl, uint8_t rst) // 16
	: SSD1306(_address, _sda, _scl)
{
	pinMode(rst, OUTPUT); 
	digitalWrite(rst, LOW); // set rst low to reset OLED 
	delay(50);
	digitalWrite(rst, HIGH); // while OLED is running, must set rst to high 
	  
	init();
	flipScreenVertically();
	setFont(ArialMT_Plain_24);

	setColor(WHITE);
	setTextAlignment(TEXT_ALIGN_LEFT);
	//setTextColor(WHITE, BLACK);
}

void LiquidCrystalEx::Print(int x, int y, const char* str)
{
	//clear();
	drawString(5, y * 30, str);
	display();
}

void LiquidCrystalEx::Print(int x, int y, int num)
{
	//clear();
	drawString(5, y * 30, String(num));
	display();
}
