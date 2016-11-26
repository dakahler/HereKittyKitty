#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include "Util.h"
#include <Wire.h>
#include <Time.h>
#include "EEPRomAnything.h"

LiquidCrystalEx Program::s_lcd(7, 8, 9, 10, 11, 12);
Timer Program::s_updateLcdTimer(1000, Program::UpdateLcd);
Timer Program::s_motorFeedTimer(Program::feedSpeed, Program::StepMotor, Program::feedSteps, false,
	Program::StartMotor, Program::StopMotor);
Timer Program::s_exitSettingsTimer(60 * 1000, Program::ExitSettings, 1, false);
ButtonPress Program::s_actionButton(26, Program::DoAction);
ButtonPress Program::s_changePageButton(28, Program::ChangePage);
RTC_DS1307 Program::s_rtc;
EasyDriver Program::s_easyDriver(2, 3, 4, 5, 6, false);
uint8_t Program::s_feedHours[12];
int Program::s_currentFeedIndex;
int Program::s_previousLoopHour;
Program* Program::m_instance = NULL;
Program::Page Program::s_currentPage = Program::Page::Main;
Program::eepromData Program::s_eepromData;

Program* Program::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Program();
	}

	return m_instance;
}

Program::Program()
{
	short existingVersion;
	EEPROM_readAnything(0, existingVersion);
	if (s_eepromData.Version != existingVersion)
	{
		EEPROM_writeAnything(0, s_eepromData);
	}
	else
	{
		EEPROM_readAnything(0, s_eepromData);
	}

	Serial.begin(9600);
	Wire.begin();
	s_rtc.begin();
	if (!s_rtc.isrunning())
	{
		// TODO: Need better way of syncing time
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		s_rtc.adjust(DateTime(__DATE__, __TIME__));
	}

	const int feedInterval = 24 / feedNumDaily;
	bool setCurrentFeedIndex = false;
	for (int i = 0; i < feedNumDaily; i++)
	{
		s_feedHours[i] = feedStartTime + (feedInterval * i);

		if (!setCurrentFeedIndex)
		{
			if (s_feedHours[i] >= s_rtc.now().hour())
			{
				setCurrentFeedIndex = true;
				s_currentFeedIndex = i;
			}
		}
	}

	s_previousLoopHour = s_rtc.now().hour();

	s_lcd.begin(16, 2);
	s_updateLcdTimer.FireNow();
}

void Program::Update()
{
	DateTime now = s_rtc.now();
	if (now.hour() != s_previousLoopHour)
	{
		s_previousLoopHour = now.hour();
		if (s_feedHours[s_currentFeedIndex] == now.hour())
		{
			// Food!
			DoAction();
			s_currentFeedIndex++;
			if (s_currentFeedIndex >= feedNumDaily)
			{
				s_currentFeedIndex = 0;
			}
		}
	}

	s_updateLcdTimer.Update();
	s_motorFeedTimer.Update();
	s_actionButton.Update();
	s_exitSettingsTimer.Update();
}

void Program::DoAction()
{
	if (s_currentPage == Page::Main)
	{
		s_motorFeedTimer.Restart();
	}
	else if (s_currentPage == Page::OuncesPerMeal)
	{
		s_eepromData.OuncesPerMeal = (s_eepromData.OuncesPerMeal + 1) / maxOuncesPerMeal;
	}
	else if (s_currentPage == Page::MealsPerDay)
	{
		s_eepromData.MealsPerDay = (s_eepromData.MealsPerDay + 1) / maxMealsPerDay;
	}
}

void Program::ChangePage()
{
	s_currentPage = (Page)((s_currentPage + 1) % Page::NumPages);
	s_exitSettingsTimer.Restart();
}

void Program::UpdateLcd()
{
	if (s_currentPage == Page::Main)
	{
		DateTime now = s_rtc.now();

		char timeString[18];
		sprintf(timeString, "%02d:%02d:%02d %s",
			hourFormat12(now.hour()), now.minute(), now.second(), isAM(now.hour()) ? "AM" : "PM");

		s_lcd.Print(0, 0, timeString);
		s_lcd.Print(13, 0, feedOunces);
		s_lcd.Print(15, 0, feedNumDaily);

		sprintf(timeString, "Next: %02d:00 %s", hourFormat12(s_feedHours[s_currentFeedIndex]),
			isAM(s_feedHours[s_currentFeedIndex]) ? "AM" : "PM");

		s_lcd.Print(0, 1, timeString);
	}
	else if (s_currentPage == Page::OuncesPerMeal)
	{
		s_lcd.Print(0, 0, "Ounces Per Meal:");
		s_lcd.Print(0, 1, s_eepromData.OuncesPerMeal);
	}
	else if (s_currentPage == Page::MealsPerDay)
	{
		s_lcd.Print(0, 0, "Meals Per Day:");
		s_lcd.Print(0, 1, s_eepromData.MealsPerDay);
	}
}

void Program::StartMotor()
{
	s_easyDriver.EnableMotor();
	s_easyDriver.SetDirection(true);
}

void Program::StopMotor()
{
	s_easyDriver.DisableMotor();
}

void Program::StepMotor()
{
	s_easyDriver.Step();
}

void Program::ExitSettings()
{
	s_currentPage = Page::Main;
	EEPROM_writeAnything(0, s_eepromData);
}
