#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include "Util.h"
#include <Wire.h>
#include <Time.h>
#include "EEPRomAnything.h"
#include "AudioPlayer.h"

LiquidCrystalEx Program::s_lcd(7, 8, 9, 10, 11, 12);
Timer Program::s_updateLcdTimer(1000ul, Program::UpdateLcd);
Timer Program::s_motorFeedTimer(Program::feedSpeed, Program::StepMotor, 100, false,
	Program::StartMotor, Program::StopMotor);
Timer Program::s_exitSettingsTimer(60ul * 1000ul, Program::ExitSettings, 1, false);
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



AudioPlayer::Note notes[] =
{
	AudioPlayer::Note(Notes::NOTE_E7, 12), AudioPlayer::Note(Notes::NOTE_E7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_E7, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_C7, 12), AudioPlayer::Note(Notes::NOTE_E7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),
	AudioPlayer::Note(Notes::NOTE_G7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),
	AudioPlayer::Note(Notes::NOTE_G6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),

	AudioPlayer::Note(Notes::NOTE_C7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_G6, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_E6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_A6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_B6, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_AS6, 12), AudioPlayer::Note(Notes::NOTE_A6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),

	AudioPlayer::Note(Notes::NOTE_G6, 9), AudioPlayer::Note(Notes::NOTE_E7, 9), AudioPlayer::Note(Notes::NOTE_G7, 9),
	AudioPlayer::Note(Notes::NOTE_A7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_F7, 12), AudioPlayer::Note(Notes::NOTE_G7, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_E7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_C7, 12),
	AudioPlayer::Note(Notes::NOTE_D7, 12), AudioPlayer::Note(Notes::NOTE_B6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),

	AudioPlayer::Note(Notes::NOTE_C7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_G6, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_E6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_A6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_B6, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_AS6, 12), AudioPlayer::Note(Notes::NOTE_A6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),

	AudioPlayer::Note(Notes::NOTE_G6, 9), AudioPlayer::Note(Notes::NOTE_E7, 9), AudioPlayer::Note(Notes::NOTE_G7, 9),
	AudioPlayer::Note(Notes::NOTE_A7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_F7, 12), AudioPlayer::Note(Notes::NOTE_G7, 12),
	AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_E7, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_C7, 12),
	AudioPlayer::Note(Notes::NOTE_D7, 12), AudioPlayer::Note(Notes::NOTE_B6, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12), AudioPlayer::Note(Notes::NOTE_SILENCE, 12),
};

AudioPlayer::Tune tune(notes, sizeof(notes) / sizeof(notes[0]));
AudioPlayer audioPlayer(tune, 30);

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

	RecalculateMealTimes();

	s_previousLoopHour = s_rtc.now().hour();

	s_lcd.begin(16, 2);
	s_updateLcdTimer.FireNow();
}

void Program::RecalculateMealTimes()
{
	const int feedInterval = 24 / s_eepromData.MealsPerDay;
	bool setCurrentFeedIndex = false;
	for (int i = 0; i < s_eepromData.MealsPerDay; i++)
	{
		s_feedHours[i] = s_eepromData.StartHour + (feedInterval * i);

		if (!setCurrentFeedIndex)
		{
			if (s_feedHours[i] > s_rtc.now().hour())
			{
				setCurrentFeedIndex = true;
				s_currentFeedIndex = i;
			}
		}
	}
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
			if (s_currentFeedIndex >= s_eepromData.MealsPerDay)
			{
				s_currentFeedIndex = 0;
			}
		}
	}

	s_updateLcdTimer.Update();
	s_motorFeedTimer.Update();
	s_actionButton.Update();
	s_changePageButton.Update();
	s_exitSettingsTimer.Update();
	audioPlayer.Update();
}

void Program::DoAction()
{
	if (s_currentPage == Page::Main)
	{
		s_motorFeedTimer.SetIterations((s_eepromData.OuncesPerMeal / ouncesPerCompartment) * stepsPerCompartment);
		s_motorFeedTimer.Restart();
	}
	else if (s_currentPage == Page::OuncesPerMeal)
	{
		s_eepromData.OuncesPerMeal = (s_eepromData.OuncesPerMeal + 1) % (maxOuncesPerMeal + 1);
		if (s_eepromData.OuncesPerMeal == 0)
		{
			s_eepromData.OuncesPerMeal = 1;
		}

		RecalculateMealTimes();
		UpdateLcd();
	}
	else if (s_currentPage == Page::MealsPerDay)
	{
		s_eepromData.MealsPerDay = (s_eepromData.MealsPerDay + 1) % (maxMealsPerDay + 1);
		if (s_eepromData.MealsPerDay == 0)
		{
			s_eepromData.MealsPerDay = 1;
		}

		RecalculateMealTimes();
		UpdateLcd();
	}
	else if (s_currentPage == Page::StartHour)
	{
		s_eepromData.StartHour = (s_eepromData.StartHour + 1) % 24;
		if (s_eepromData.StartHour == 0)
		{
			s_lcd.clear();
		}

		RecalculateMealTimes();
		UpdateLcd();
	}
}

void Program::ChangePage()
{
	s_currentPage = (Page)((s_currentPage + 1) % Page::NumPages);
	s_exitSettingsTimer.Restart();
	s_lcd.clear();
	UpdateLcd();
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
	else if (s_currentPage == Page::StartHour)
	{
		s_lcd.Print(0, 0, "Start Hour:");
		s_lcd.Print(0, 1, s_eepromData.StartHour);
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
	audioPlayer.Play();
}

void Program::StepMotor()
{
	s_easyDriver.Step();
}

void Program::ExitSettings()
{
	s_currentPage = Page::Main;
	s_lcd.clear();
	EEPROM_writeAnything(0, s_eepromData);
}
