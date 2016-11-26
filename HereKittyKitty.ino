#include "AudioPlayer.h"
#include "Program.h"

void setup() { }

void loop()
{
	Program::GetInstance()->Update();
}
