#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <Time.h>
#include "EEPRomAnything.h"
#include "Util.h"
#include "PageFactory.h"

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
	m_updateLcdTimer(1000ul, MethodSlot<Program, const Timer<Program>&>(this, &Program::UpdateLcd)),
	m_exitSettingsTimer(60ul * 1000ul, MethodSlot<Program, const Timer<Program>&>(this, &Program::ExitSettings), 1, false),
	m_actionButton(26, MethodSlot<Program, const ButtonPress<Program>&>(this, &Program::DoAction)),
	m_changePageButton(28, MethodSlot<Program, const ButtonPress<Program>&>(this, &Program::ChangePage))
{
	m_ouncesPerMealPage = PageFactory::Create<OuncesPerMealPage>();
	m_mealsPerDayPage = PageFactory::Create<MealsPerDayPage>();
	m_startHourPage = PageFactory::Create<StartHourPage>();
	m_mainPage = PageFactory::Create<MainPage>(m_ouncesPerMealPage->m_ouncesPerMeal);

	Load();

	m_currentPage = m_mainPage;

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

	const int feedInterval = 24 / m_mealsPerDayPage->GetMealsPerDay();
	for (int i = 0; i < m_mealsPerDayPage->GetMealsPerDay(); i++)
	{
		m_feedHours[i] = (m_startHourPage->GetStartHour() + (feedInterval * i)) % 24;
		Serial.print("  ");
		Serial.print(m_feedHours[i]);
		Serial.println();
	}

	// Look for the next meal time
	for (int i = m_rtc.now().hour() + 1, j = 0; j < 24; i++, j++)
	{
		i %= 24;
		for (int k = 0; k < m_mealsPerDayPage->GetMealsPerDay(); k++)
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
	for (const IPage* page : PageFactory::GetPages())
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
		for (IPage* page : PageFactory::GetPages())
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
			DoAction(m_actionButton);
			m_currentFeedIndex++;
			if (m_currentFeedIndex >= m_mealsPerDayPage->GetMealsPerDay())
			{
				m_currentFeedIndex = 0;
			}
		}
	}

	for (IPage* page : PageFactory::GetPages())
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

void Program::DoAction(const ButtonPress<Program>& button)
{
	Program* program = Program::GetInstance();
	program->m_currentPage->InvokeAction();
	program->RecalculateMealTimes();
	program->m_currentPage->UpdateLcd(program->m_lcd);
}

void Program::ChangePage(const ButtonPress<Program>& button)
{
	Program* program = Program::GetInstance();
	for (unsigned int i = 0; i < PageFactory::GetPages().size(); i++)
	{
		if (program->m_currentPage == PageFactory::GetPages()[i])
		{
			unsigned int nextPage = i + 1;
			if (nextPage == PageFactory::GetPages().size())
			{
				nextPage = 0;
			}

			program->m_currentPage = PageFactory::GetPages()[nextPage];
			break;
		}
	}

	program->m_exitSettingsTimer.Restart();
	program->m_lcd.clear();
	UpdateLcd(program->m_updateLcdTimer);
}

void Program::UpdateLcd(const Timer<Program>& timer)
{
	Program* program = Program::GetInstance();
	program->m_currentPage->UpdateLcd(program->m_lcd);
}

void Program::ExitSettings(const Timer<Program>& timer)
{
	Program* program = Program::GetInstance();
	program->m_currentPage = program->m_mainPage;
	program->m_lcd.clear();
	program->Save();
}
