#include "MainPage.h"
#include "Page.h"
#include "AudioPlayer.h"
#include "Program.h"

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	Program::GetInstance()->Update();
}
