#include <SH1106Brzo.h>
#include <SH1106Spi.h>
#include <SH1106Wire.h>
#include <SSD1306.h>
#include <SSD1306Brzo.h>
#include <SSD1306Spi.h>
#include <SSD1306Wire.h>

#include "PageFactory.h"
#include "MainPage.h"
#include "Page.h"
#include "AudioPlayer.h"
#include "Program.h"

void setup()
{
	Serial.begin(115200);
}

void loop()
{
	Program::GetInstance()->Update();
}
