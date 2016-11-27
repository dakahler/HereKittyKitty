#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <Time.h>
#include "EEPRomAnything.h"
#include "Util.h"

Program* Program::m_instance = NULL;
Program* Program::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Program();
		if (m_instance == NULL)
		{
			Serial.println("Failed to allocate Program!");
		}
	}

	return m_instance;
}

Program::Program()
	: m_lcd(7, 8, 9, 10, 11, 12),
	m_updateLcdTimer(1000ul, Program::UpdateLcd),
	m_exitSettingsTimer(60ul * 1000ul, Program::ExitSettings, 1, false),
	m_actionButton(26, Program::DoAction),
	m_changePageButton(28, Program::ChangePage),
	m_ouncesPerMealPage(),
	m_mainPage(m_ouncesPerMealPage.m_ouncesPerMeal)
{
	m_pages[0] = &m_mainPage;
	m_pages[1] = &m_ouncesPerMealPage;
	m_pages[2] = &m_mealsPerDayPage;
	m_pages[3] = &m_startHourPage;

	Load();

	m_currentPage = &m_mainPage;

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
}

void Program::RecalculateMealTimes()
{
	Serial.println("Recalculating...");

	const int feedInterval = 24 / m_mealsPerDayPage.GetMealsPerDay();
	for (int i = 0; i < m_mealsPerDayPage.GetMealsPerDay(); i++)
	{
		m_feedHours[i] = (m_startHourPage.GetStartHour() + (feedInterval * i)) % 24;
		Serial.print("  ");
		Serial.print(m_feedHours[i]);
		Serial.println();
	}

	// Look for the next meal time
	for (int i = m_rtc.now().hour() + 1, j = 0; j < 24; i++, j++)
	{
		i %= 24;
		for (int k = 0; k < m_mealsPerDayPage.GetMealsPerDay(); k++)
		{
			if (m_feedHours[k] == i)
			{
				m_currentFeedIndex = k;
				Serial.print("Next meal hour: ");
				Serial.print(i);
				Serial.println();
				return;
			}
		}
	}
}

void Program::Save()
{
	int offset = EEPROM_writeAnything(0, Version);
	for (IPage* page : m_pages)
	{
		offset += page->WriteToEepRom(offset);
	}
}

void Program::Load()
{
	short existingVersion;
	int offset = EEPROM_readAnything(0, existingVersion);
	if (Version != existingVersion)
	{
		Save();
	}
	else
	{
		for (IPage* page : m_pages)
		{
			offset += page->ReadFromEepRom(offset);
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
			if (m_currentFeedIndex >= m_mealsPerDayPage.GetMealsPerDay())
			{
				m_currentFeedIndex = 0;
			}
		}
	}

	for (IPage* page : m_pages)
	{
		page->Update();
	}

	m_updateLcdTimer.Update();
	m_actionButton.Update();
	m_changePageButton.Update();
	m_exitSettingsTimer.Update();
}

int Program::GetNextMealHour() const
{
	return m_feedHours[m_currentFeedIndex];
}

void Program::DoAction()
{
	Program* program = Program::GetInstance();
	program->m_currentPage->InvokeAction();
	program->RecalculateMealTimes();
	program->m_currentPage->UpdateLcd(program->m_lcd);
}

void Program::ChangePage()
{
	Program* program = Program::GetInstance();
	for (unsigned int i = 0; i < COUNT_OF(program->m_pages); i++)
	{
		if (program->m_currentPage == program->m_pages[i])
		{
			int nextPage = i + 1;
			if (nextPage == COUNT_OF(program->m_pages))
			{
				nextPage = 0;
			}

			program->m_currentPage = program->m_pages[nextPage];
			break;
		}
	}

	program->m_exitSettingsTimer.Restart();
	program->m_lcd.clear();
	UpdateLcd();
}

void Program::UpdateLcd()
{
	Program* program = Program::GetInstance();
	program->m_currentPage->UpdateLcd(program->m_lcd);
}

void Program::ExitSettings()
{
	Program* program = Program::GetInstance();
	program->m_currentPage = program->m_pages[0];
	program->m_lcd.clear();
	program->Save();
}
