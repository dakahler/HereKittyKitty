#include "MainPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "Song_SMB.h"
#include "LiquidCrystalEx.h"
#include "Program.h"

EasyDriver MainPage::s_easyDriver(2, 3, 4, 5, 6, false);
AudioPlayer MainPage::s_audioPlayer(song_smb, 30);
bool MainPage::isFeeding = false;

// TODO: Taking a reference from another page for a value here is really hacky
MainPage::MainPage(byte& ouncesPerMeal)
	: m_motorFeedTimer(c_feedSpeed, MainPage::StepMotor, 100, false,
		MainPage::StartMotor, MainPage::StopMotor),
	m_ouncesPerMeal(ouncesPerMeal)
{
	
}

void MainPage::Update()
{
	m_motorFeedTimer.Update();
	s_audioPlayer.Update();
}

int MainPage::WriteToEepRom(int offset)
{
	return 0;
}

int MainPage::ReadFromEepRom(int offset)
{
	return 0;
}

void MainPage::InvokeAction()
{
	m_motorFeedTimer.SetIterations((m_ouncesPerMeal / c_ouncesPerCompartment) * c_stepsPerCompartment);
	m_motorFeedTimer.Restart();
}

void MainPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	DateTime now = RTC_DS1307::now();

	char timeString[18];
	sprintf(timeString, "%02d:%02d:%02d %s",
		hourFormat12(now.hour()), now.minute(), now.second(), isAM(now.hour()) ? "AM" : "PM");

	lcd.Print(0, 0, timeString);

	int nextMealHour = Program::GetInstance()->GetNextMealHour();
	sprintf(timeString, "Next: %02d:00 %s", hourFormat12(nextMealHour), isAM(nextMealHour) ? "AM" : "PM");

	lcd.Print(0, 1, timeString);

	if (isFeeding)
	{
		lcd.Print(13, 0, "   ");
		lcd.Print(13, 0, m_motorFeedTimer.GetIterations());
	}
	else
	{
		lcd.Print(13, 0, "   ");
	}
}

void MainPage::StartMotor()
{
	isFeeding = true;
	s_easyDriver.EnableMotor();
	s_easyDriver.SetDirection(true);
}

void MainPage::StopMotor()
{
	isFeeding = false;
	s_easyDriver.DisableMotor();
	s_audioPlayer.Play();
}

void MainPage::StepMotor()
{
	s_easyDriver.Step();
}
