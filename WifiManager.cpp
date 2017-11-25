#include "WifiManager.h"
#include "esp_wps.h"

/*
Change the definition of the WPS mode
from WPS_TYPE_PBC to WPS_TYPE_PIN in
the case that you are using pin type
WPS
*/
#define ESP_WPS_MODE WPS_TYPE_PBC

esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(ESP_WPS_MODE);

Preferences* WifiManager::preferences = nullptr;
WifiManager* WifiManager::m_instance = nullptr;
WifiManager* WifiManager::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new WifiManager();
		if (m_instance == NULL)
		{
			Serial.println("Failed to allocate Program!");
		}
	}

	return m_instance;
}

WifiManager::WifiManager()
{
    
}

void WifiManager::Connect(Preferences* preferences)
{
    assert(preferences);
    WifiManager::preferences = preferences;

    WiFi.mode(WIFI_MODE_STA);
	delay(1000);
	WiFi.persistent(true);
	WiFi.begin(preferences->getString("SSID", "").c_str(), preferences->getString("Passphrase", "").c_str());

    for (int i = 9; i < 20 && WiFi.status() == WL_DISCONNECTED; i++)
    {
        delay(500);
        Serial.print(".");
    }

    // Check if WiFi is already connected and if not, begin the WPS process.
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.onEvent(WiFiEvent);

        // WPS button I/O setup
        pinMode(0, OUTPUT);       // Use GPIO0
        digitalWrite(0, LOW);     // for hardware safe operation.
        pinMode(2, INPUT_PULLUP); // Push Button for GPIO2 active LOW

        Serial.println(String("Starting WPS ") + WiFi.SSID().c_str());

        esp_wifi_wps_enable(&config);
        esp_wifi_wps_start(0);
    }
    else
    {
        Serial.println(String("Connected to existing WiFi network ") + WiFi.SSID().c_str() + ".");
    }
}

String WifiManager::WPSPinToString(uint8_t a[])
{
    char wps_pin[9];
    for (int i = 0; i < 8; i++)
    {
        wps_pin[i] = a[i];
    }
    wps_pin[8] = '\0';
    return (String)wps_pin;
}

void WifiManager::WiFiEvent(WiFiEvent_t event, system_event_info_t info)
{
    switch (event)
    {
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
        WifiManager::preferences->putString("SSID", WiFi.SSID().c_str());
        WifiManager::preferences->putString("Passphrase", WiFi.psk().c_str());
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
        Serial.println("WPS_PIN = " + WPSPinToString(info.sta_er_pin.pin_code));
        break;
    default:
        break;
    }
}
