#include "StartHourPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"

StartHourPage::StartHourPage()
{

}

void StartHourPage::Update()
{
	
}

int StartHourPage::WriteToEepRom(int offset)
{
	return EEPROM_writeAnything(offset, m_startHour);
}

int StartHourPage::ReadFromEepRom(int offset)
{
	return EEPROM_readAnything(offset, m_startHour);
}

void StartHourPage::InvokeAction()
{
	m_startHour = (m_startHour + 1) % 24;
}

void StartHourPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.Print(0, 0, "Start Hour:");
	lcd.Print(0, 1, m_startHour);
}
