#include "Program.h"
#include "ButtonPress.h"
#include "Timer.h"
#include <Wire.h>
#include <TimeLib.h>
#include <Preferences.h>
#include "Util.h"
#include "PageFactory.h"
#include "NTPManager.h"
#include "WiFi.h"
#include "esp_wps.h"

/*
Change the definition of the WPS mode
from WPS_TYPE_PBC to WPS_TYPE_PIN in
the case that you are using pin type
WPS
*/
#define ESP_WPS_MODE WPS_TYPE_PBC

esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(ESP_WPS_MODE);
Preferences preferences;

String wpspin2string(uint8_t a[]){
	char wps_pin[9];
	for(int i=0;i<8;i++){
	  wps_pin[i] = a[i];
	}
	wps_pin[8] = '\0';
	return (String)wps_pin;
  }

  void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
	switch(event){
	  case SYSTEM_EVENT_STA_START:
	  Serial.println("Station Mode Started");
	  break;
	  case SYSTEM_EVENT_STA_GOT_IP:
	  Serial.println("Connected to :" + String(WiFi.SSID()));
	  Serial.print("Got IP: ");
	  Serial.println(WiFi.localIP());
	  break;
	  case SYSTEM_EVENT_STA_DISCONNECTED:
	  Serial.println("Disconnected from station, attempting reconnection");
	  WiFi.reconnect();
	  break;
	  case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
	  Serial.println("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()) + " " + WiFi.psk().c_str());
	  preferences.putString("SSID", WiFi.SSID().c_str());
	  preferences.putString("Passphrase", WiFi.psk().c_str());
	  esp_wifi_wps_disable();
	  delay(10);
	  WiFi.begin();
	  break;
	  case SYSTEM_EVENT_STA_WPS_ER_FAILED:
	  Serial.println("WPS Failed, retrying");
	  esp_wifi_wps_disable();
	  esp_wifi_wps_enable(&config);
	  esp_wifi_wps_start(0);
	  break;
	  case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
	  Serial.println("WPS Timedout, retrying");
	  esp_wifi_wps_disable();
	  esp_wifi_wps_enable(&config);
	  esp_wifi_wps_start(0);
	  break;
	  case SYSTEM_EVENT_STA_WPS_ER_PIN:
	  Serial.println("WPS_PIN = " + wpspin2string(info.sta_er_pin.pin_code));
	  break;
	  default:
	  break;
	}
  }



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

	WiFi.mode(WIFI_MODE_STA);
	delay(1000);
	WiFi.persistent(true);
	WiFi.begin(preferences.getString("SSID", "").c_str(), preferences.getString("Passphrase", "").c_str());

	for (int i = 9; i < 20 && WiFi.status() == WL_DISCONNECTED; i++) {          // last saved credentials
		delay(500);
		Serial.print(".");
	  }

	// Check if WiFi is already connected and if not, begin the WPS process. 
	if (WiFi.status() != WL_CONNECTED) {
	WiFi.onEvent(WiFiEvent);
	
	// WPS button I/O setup
    pinMode(0,OUTPUT);         // Use GPIO0
    digitalWrite(0,LOW);       // for hardware safe operation.
    pinMode(2, INPUT_PULLUP);  // Push Button for GPIO2 active LOW

	Serial.println(String("Starting WPS ") + WiFi.SSID().c_str());
  
	esp_wifi_wps_enable(&config);
	esp_wifi_wps_start(0);
	}
	else
	{
		Serial.println(String("Connected to existing WiFi network ") + WiFi.SSID().c_str() + ".");
	}

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
	//Serial.println("update");
	//Serial.print(m_currentFeedIndex);
	time_t t = now(); // = m_rtc.now();
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

	//Serial.println("updating pages");
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
