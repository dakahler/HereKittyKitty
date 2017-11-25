#ifndef _NTPMANAGER_h
#define _NTPMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"
#include <WiFiUdp.h>
#include <TimeLib.h>

class NTPManager
{
public:
	static NTPManager* GetInstance();
    void Connect();

private:
    NTPManager();
    static time_t GetTime();
    void SendPacket(IPAddress &address);

    static NTPManager* m_instance;

    WiFiUDP Udp;

    const String ntpServerName = "us.pool.ntp.org";
    const int timeZone = -8;  // Pacific Standard Time (USA)
    //const int timeZone = -7;  // Pacific Daylight Time (USA)
    const unsigned int localPort = 8888;  // local port to listen for UDP packets

    
    
};

#endif // _NTPMANAGER_h
