#include "StartHourPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"
#include <Preferences.h>

StartHourPage::StartHourPage()
{

}

void StartHourPage::Update()
{
	
}

void StartHourPage::WriteToEepRom(Preferences& preferences) const
{
	preferences.putChar("StartHour", m_startHour);
}

void StartHourPage::ReadFromEepRom(Preferences& preferences)
{
	m_startHour = preferences.getChar("StartHour", 6);
	Serial.println(String("  Start Hour: ") + m_startHour);
}

void StartHourPage::InvokeAction()
{
	m_startHour = (m_startHour + 1) % 24;
}

void StartHourPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.clear();
	lcd.Print(0, 0, "Start Hour:");
	lcd.Print(0, 1, m_startHour);
}
