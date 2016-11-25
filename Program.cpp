#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include "Util.h"
#include <Wire.h>
#include <Time.h>
#include <pnew.cpp>

LiquidCrystalEx Program::s_lcd(7, 8, 9, 10, 11, 12);
Timer Program::s_updateLcdTimer(1000, Program::UpdateLcd);
Timer Program::s_motorFeedTimer(Program::feedSpeed, Program::StepMotor, Program::feedSteps * 8, false,
	Program::StartMotor, Program::StopMotor);
ButtonPress Program::s_feedNowButton(26, Program::FeedNow);
RTC_DS1307 Program::s_rtc;
EasyDriver Program::s_easyDriver(2, 3, 4, 5, 6);
uint8_t Program::s_feedHours[12];
int Program::s_currentFeedIndex;
int Program::s_previousLoopHour;
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
{
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
			FeedNow();
			s_currentFeedIndex++;
			if (s_currentFeedIndex >= feedNumDaily)
			{
				s_currentFeedIndex = 0;
			}
		}
	}

	s_updateLcdTimer.Update();
	s_motorFeedTimer.Update();
	s_feedNowButton.Update();
}

void Program::FeedNow()
{
	s_motorFeedTimer.Restart();
}

void Program::UpdateLcd()
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
