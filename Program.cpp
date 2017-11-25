#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <TimeLib.h>
#include <Preferences.h>
#include "Util.h"
#include "PageFactory.h"
#include "NTPManager.h"
#include "WifiManager.h"

Preferences preferences;

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
	: m_lcd(0x3c, 4, 15, 16),
	m_updateLcdTimer(1000ul, MethodSlot<Program, const Timer<Program>&>(this, &Program::UpdateLcd)),
	m_exitSettingsTimer(60ul * 1000ul, MethodSlot<Program, const Timer<Program>&>(this, &Program::ExitSettings), 1, false),
	m_actionButton(12, MethodSlot<Program, const ButtonPress<Program>&>(this, &Program::DoAction)),
	m_changePageButton(14, MethodSlot<Program, const ButtonPress<Program>&>(this, &Program::ChangePage))
{
	m_ouncesPerMealPage = PageFactory::Create<OuncesPerMealPage>();
	m_mealsPerDayPage = PageFactory::Create<MealsPerDayPage>();
	m_startHourPage = PageFactory::Create<StartHourPage>();
	m_mainPage = PageFactory::Create<MainPage>(m_ouncesPerMealPage->m_ouncesPerMeal);

	Load();

	m_currentPage = m_mainPage;

	WifiManager::GetInstance()->Connect(&preferences);
	NTPManager::GetInstance()->Connect();

	RecalculateMealTimes();
	
	m_previousLoopHour = hour();
}

void Program::RecalculateMealTimes()
{
	const int feedInterval = 24 / m_mealsPerDayPage->GetMealsPerDay();
	for (int i = 0; i < m_mealsPerDayPage->GetMealsPerDay(); i++)
	{
		m_feedHours[i] = (m_startHourPage->GetStartHour() + (feedInterval * i)) % 24;
	}

	// Look for the next meal time
	for (int i = hour() + 1, j = 0; j < 24; i++, j++)
	{
		i %= 24;
		for (int k = 0; k < m_mealsPerDayPage->GetMealsPerDay(); k++)
		{
			if (m_feedHours[k] == i)
			{
				m_currentFeedIndex = k;
				Serial.print("  Next meal hour: ");
				Serial.print(i);
				Serial.println();
				return;
			}
		}
	}
}

void Program::Save()
{
	preferences.putShort("Version", Version);
	for (const IPage* page : PageFactory::GetPages())
	{
		page->WriteToEepRom(preferences);
	}
}

void Program::Load()
{
	preferences.begin("herekittykitty", false);

	// Remove all preferences under opened namespace
  	//preferences.clear();

	short existingVersion = preferences.getShort("Version", -1);
	if (Version != existingVersion)
	{
		Serial.println("Version mismatch!");
		Save();
	}
	else
	{
		Serial.println("Loading preferences...");
		for (IPage* page : PageFactory::GetPages())
		{
			page->ReadFromEepRom(preferences);
		}
	}
}

void Program::Update()
{
	time_t t = now();
	if (hour(t) != m_previousLoopHour)
	{
		m_previousLoopHour = hour(t);
		if (m_feedHours[m_currentFeedIndex] == hour(t))
		{
			// Food!
			Serial.println("Food!");
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
