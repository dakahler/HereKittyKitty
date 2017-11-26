#include "MealsPerDayPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"
#include <Preferences.h>

MealsPerDayPage::MealsPerDayPage()
{

}

void MealsPerDayPage::Update()
{
	
}

void MealsPerDayPage::WriteToEepRom(Preferences& preferences) const
{
	preferences.putChar("MealsPerDay", m_mealsPerDay);
}

void MealsPerDayPage::ReadFromEepRom(Preferences& preferences)
{
	m_mealsPerDay = preferences.getChar("MealsPerDay", 4);
	Serial.println(String("  Meals Per Day: ") + m_mealsPerDay);
}

void MealsPerDayPage::InvokeAction()
{
	m_mealsPerDay = (m_mealsPerDay + 1) % (c_maxMealsPerDay + 1);
	if (m_mealsPerDay == 0)
	{
		m_mealsPerDay = 1;
	}

	Serial.println(String("New meals per day: ") + m_mealsPerDay);
}

void MealsPerDayPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.clear();
	lcd.Print(0, 0, "Meals Per Day");
	lcd.Print(0, 1, m_mealsPerDay);
}
