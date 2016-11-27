#include "MealsPerDayPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"

MealsPerDayPage::MealsPerDayPage()
{

}

void MealsPerDayPage::Update()
{
	
}

int MealsPerDayPage::WriteToEepRom(int offset)
{
	return EEPROM_writeAnything(offset, m_mealsPerDay);
}

int MealsPerDayPage::ReadFromEepRom(int offset)
{
	return EEPROM_readAnything(offset, m_mealsPerDay);
}

void MealsPerDayPage::InvokeAction()
{
	m_mealsPerDay = (m_mealsPerDay + 1) % (c_maxMealsPerDay + 1);
	if (m_mealsPerDay == 0)
	{
		m_mealsPerDay = 1;
	}
}

void MealsPerDayPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.Print(0, 0, "Meals Per Day:");
	lcd.Print(0, 1, m_mealsPerDay);
}
