#include "MainPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "Song_SMB.h"
#include "LiquidCrystalEx.h"
#include "Program.h"

// TODO: Taking a reference from another page for a value here is really hacky
MainPage::MainPage(byte& ouncesPerMeal)
	: m_motorFeedTimer(c_feedSpeed, MethodSlot<MainPage, const Timer<MainPage>&>(this, &MainPage::StepMotor), 100, false,
		MethodSlot<MainPage, const Timer<MainPage>&>(this, &MainPage::StartMotor),
		MethodSlot<MainPage, const Timer<MainPage>&>(this, &MainPage::StopMotor)),
	m_ouncesPerMeal(ouncesPerMeal),
	m_isFeeding(false),
	m_easyDriver(2, 3, 4, 5, 52, 6, true),
	m_audioPlayer(song_smb, 30)
{
	
}

void MainPage::Update()
{
	m_motorFeedTimer.Update();
	m_audioPlayer.Update();
}

int MainPage::WriteToEepRom(int offset) const
{
	return 0;
}

int MainPage::ReadFromEepRom(int offset) const
{
	return 0;
}

void MainPage::InvokeAction()
{
	int iterations = (m_ouncesPerMeal / c_ouncesPerCompartment) * c_stepsPerCompartment * 2;
	m_motorFeedTimer.SetIterations(iterations);
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

	if (m_isFeeding)
	{
		lcd.Print(13, 0, "   ");
		lcd.Print(13, 0, m_motorFeedTimer.GetIterations());
	}
	else
	{
		lcd.Print(13, 0, "   ");
	}
}

void MainPage::StartMotor(const Timer<MainPage>& timer)
{
	m_isFeeding = true;
	m_easyDriver.EnableMotor();
	m_easyDriver.SetDirection(true);
}

void MainPage::StopMotor(const Timer<MainPage>& timer)
{
	m_isFeeding = false;
	m_easyDriver.DisableMotor();
	m_audioPlayer.Play();
}

void MainPage::StepMotor(const Timer<MainPage>& timer)
{
	m_easyDriver.Step();
}
