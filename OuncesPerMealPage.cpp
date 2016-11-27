#include "OuncesPerMealPage.h"
#include "Util.h"
#include "EEPRomAnything.h"
#include "LiquidCrystalEx.h"
#include "Program.h"

OuncesPerMealPage::OuncesPerMealPage()
{

}

void OuncesPerMealPage::Update()
{
	
}

int OuncesPerMealPage::WriteToEepRom(int offset)
{
	return EEPROM_writeAnything(offset, m_ouncesPerMeal);
}

int OuncesPerMealPage::ReadFromEepRom(int offset)
{
	return EEPROM_readAnything(offset, m_ouncesPerMeal);
}

void OuncesPerMealPage::InvokeAction()
{
	m_ouncesPerMeal = (m_ouncesPerMeal + 1) % (c_maxOuncesPerMeal + 1);
	if (m_ouncesPerMeal == 0)
	{
		m_ouncesPerMeal = 1;
	}
}

void OuncesPerMealPage::UpdateLcd(LiquidCrystalEx& lcd)
{
	lcd.Print(0, 0, "Ounces Per Meal:");
	lcd.Print(0, 1, m_ouncesPerMeal);
}
