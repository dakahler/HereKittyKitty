#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include "Util.h"
#include <Wire.h>
#include <Time.h>
#include "EEPRomAnything.h"
#include "Song_SMB.h"

Program* Program::m_instance = NULL;
Program* Program::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Program();
	}

	return m_instance;
}

Program::Program()
	: m_lcd(7, 8, 9, 10, 11, 12),
	m_updateLcdTimer(1000ul, Program::UpdateLcd),
	m_motorFeedTimer(Program::c_feedSpeed, Program::StepMotor, 100, false,
		Program::StartMotor, Program::StopMotor),
	m_exitSettingsTimer(60ul * 1000ul, Program::ExitSettings, 1, false),
	m_actionButton(26, Program::DoAction),
	m_changePageButton(28, Program::ChangePage),
	m_easyDriver(2, 3, 4, 5, 6, false),
	m_audioPlayer(song_smb, 30),
	m_currentPage(Program::Page::Main)
{
	short existingVersion;
	EEPROM_readAnything(0, existingVersion);
	if (m_eepromData.Version != existingVersion)
	{
		EEPROM_writeAnything(0, m_eepromData);
	}
	else
	{
		EEPROM_readAnything(0, m_eepromData);
	}

	Serial.begin(9600);
	Wire.begin();
	m_rtc.begin();
	if (!m_rtc.isrunning())
	{
		// TODO: Need better way of syncing time
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		m_rtc.adjust(DateTime(__DATE__, __TIME__));
	}

	RecalculateMealTimes();

	m_previousLoopHour = m_rtc.now().hour();

	m_lcd.begin(16, 2);
	m_updateLcdTimer.FireNow();
}

void Program::RecalculateMealTimes()
{
	const int feedInterval = 24 / m_eepromData.MealsPerDay;
	bool setCurrentFeedIndex = false;
	for (int i = 0; i < m_eepromData.MealsPerDay; i++)
	{
		m_feedHours[i] = m_eepromData.StartHour + (feedInterval * i);

		if (!setCurrentFeedIndex)
		{
			if (m_feedHours[i] > m_rtc.now().hour())
			{
				setCurrentFeedIndex = true;
				m_currentFeedIndex = i;
			}
		}
	}
}

void Program::Update()
{
	DateTime now = m_rtc.now();
	if (now.hour() != m_previousLoopHour)
	{
		m_previousLoopHour = now.hour();
		if (m_feedHours[m_currentFeedIndex] == now.hour())
		{
			// Food!
			DoAction();
			m_currentFeedIndex++;
			if (m_currentFeedIndex >= m_eepromData.MealsPerDay)
			{
				m_currentFeedIndex = 0;
			}
		}
	}

	m_updateLcdTimer.Update();
	m_motorFeedTimer.Update();
	m_actionButton.Update();
	m_changePageButton.Update();
	m_exitSettingsTimer.Update();
	m_audioPlayer.Update();
}

void Program::DoAction()
{
	Program* program = Program::GetInstance();
	if (program->m_currentPage == Page::Main)
	{
		program->m_motorFeedTimer.SetIterations((program->m_eepromData.OuncesPerMeal / program->c_ouncesPerCompartment) * program->c_stepsPerCompartment);
		program->m_motorFeedTimer.Restart();
	}
	else if (program->m_currentPage == Page::OuncesPerMeal)
	{
		program->m_eepromData.OuncesPerMeal = (program->m_eepromData.OuncesPerMeal + 1) % (program->c_maxOuncesPerMeal + 1);
		if (program->m_eepromData.OuncesPerMeal == 0)
		{
			program->m_eepromData.OuncesPerMeal = 1;
		}

		program->RecalculateMealTimes();
		UpdateLcd();
	}
	else if (program->m_currentPage == Page::MealsPerDay)
	{
		program->m_eepromData.MealsPerDay = (program->m_eepromData.MealsPerDay + 1) % (program->c_maxMealsPerDay + 1);
		if (program->m_eepromData.MealsPerDay == 0)
		{
			program->m_eepromData.MealsPerDay = 1;
		}

		program->RecalculateMealTimes();
		UpdateLcd();
	}
	else if (program->m_currentPage == Page::StartHour)
	{
		program->m_eepromData.StartHour = (program->m_eepromData.StartHour + 1) % 24;
		if (program->m_eepromData.StartHour == 0)
		{
			program->m_lcd.clear();
		}

		program->RecalculateMealTimes();
		UpdateLcd();
	}
}

void Program::ChangePage()
{
	Program* program = Program::GetInstance();
	program->m_currentPage = (Page)((program->m_currentPage + 1) % Page::NumPages);
	program->m_exitSettingsTimer.Restart();
	program->m_lcd.clear();
	UpdateLcd();
}

void Program::UpdateLcd()
{
	Program* program = Program::GetInstance();
	if (program->m_currentPage == Page::Main)
	{
		DateTime now = program->m_rtc.now();

		char timeString[18];
		sprintf(timeString, "%02d:%02d:%02d %s",
			hourFormat12(now.hour()), now.minute(), now.second(), isAM(now.hour()) ? "AM" : "PM");

		program->m_lcd.Print(0, 0, timeString);

		sprintf(timeString, "Next: %02d:00 %s", hourFormat12(program->m_feedHours[program->m_currentFeedIndex]),
			isAM(program->m_feedHours[program->m_currentFeedIndex]) ? "AM" : "PM");

		program->m_lcd.Print(0, 1, timeString);
	}
	else if (program->m_currentPage == Page::OuncesPerMeal)
	{
		program->m_lcd.Print(0, 0, "Ounces Per Meal:");
		program->m_lcd.Print(0, 1, program->m_eepromData.OuncesPerMeal);
	}
	else if (program->m_currentPage == Page::MealsPerDay)
	{
		program->m_lcd.Print(0, 0, "Meals Per Day:");
		program->m_lcd.Print(0, 1, program->m_eepromData.MealsPerDay);
	}
	else if (program->m_currentPage == Page::StartHour)
	{
		program->m_lcd.Print(0, 0, "Start Hour:");
		program->m_lcd.Print(0, 1, program->m_eepromData.StartHour);
	}
}

void Program::StartMotor()
{
	Program* program = Program::GetInstance();
	program->m_easyDriver.EnableMotor();
	program->m_easyDriver.SetDirection(true);
}

void Program::StopMotor()
{
	Program* program = Program::GetInstance();
	program->m_easyDriver.DisableMotor();
	program->m_audioPlayer.Play();
}

void Program::StepMotor()
{
	Program* program = Program::GetInstance();
	program->m_easyDriver.Step();
}

void Program::ExitSettings()
{
	Program* program = Program::GetInstance();
	program->m_currentPage = Page::Main;
	program->m_lcd.clear();
	EEPROM_writeAnything(0, program->m_eepromData);
}
