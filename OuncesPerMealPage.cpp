#include "OuncesPerMealPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"
#include <Preferences.h>

OuncesPerMealPage::OuncesPerMealPage()
{
	m_ouncesPerMeal = 4;
}

void OuncesPerMealPage::Update()
{
	
}

void OuncesPerMealPage::WriteToEepRom(Preferences& preferences) const
{
	preferences.putChar("OuncesPerMeal", m_ouncesPerMeal);
}

void OuncesPerMealPage::ReadFromEepRom(Preferences& preferences)
{
	m_ouncesPerMeal = preferences.getChar("OuncesPerMeal", 4);
	Serial.println(String("  Ounces Per Meal: ") + m_ouncesPerMeal);
}

void OuncesPerMealPage::InvokeAction()
{
	m_ouncesPerMeal = (m_ouncesPerMeal + 1) % (c_maxOuncesPerMeal + 1);
	if (m_ouncesPerMeal == 0)
	{
		m_ouncesPerMeal = 1;
	}

	Serial.println(String("New ounces per meal: ") + m_ouncesPerMeal);
}

void OuncesPerMealPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.clear();
	lcd.Print(0, 0, "Ounces Per Meal");
	lcd.Print(0, 1, m_ouncesPerMeal);
}
