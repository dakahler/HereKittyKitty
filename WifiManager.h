#ifndef _WIFIMANAGER_h
#define _WIFIMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "WiFi.h"
#include <Preferences.h>

class WifiManager
{
public:
	static WifiManager* GetInstance();
    void Connect(Preferences* preferences);

private:
    WifiManager();
    static String WPSPinToString(uint8_t a[]);
    static void WiFiEvent(WiFiEvent_t event, system_event_info_t info);
    
    static WifiManager* m_instance;
    static Preferences* preferences;
};

#endif // _WIFIMANAGER_h
