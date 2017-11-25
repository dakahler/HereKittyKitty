#include "NTPManager.h"
#include "WiFi.h"

const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

NTPManager* NTPManager::m_instance = NULL;
NTPManager* NTPManager::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new NTPManager();
		if (m_instance == NULL)
		{
			Serial.println("Failed to allocate Program!");
		}
	}

	return m_instance;
}

NTPManager::NTPManager()
{
    
}

void NTPManager::Connect()
{
    Udp.begin(localPort);
	setSyncProvider(NTPManager::GetTime);
	setSyncInterval(300);

    for (int i = 0; i < 20 && timeStatus() != timeSet; i++)
    {
        delay(500);
        Serial.print(";");
    }
}

time_t NTPManager::GetTime()
{
    IPAddress ntpServerIP; // NTP server's ip address

    while (GetInstance()->Udp.parsePacket() > 0)
        ; // discard any previously received packets
    Serial.println("Transmit NTP Request");
    // get a random server from the pool
    WiFi.hostByName(GetInstance()->ntpServerName.c_str(), ntpServerIP);
    Serial.print(GetInstance()->ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    GetInstance()->SendPacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500)
    {
        int size = GetInstance()->Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE)
        {
            Serial.println("Receive NTP Response");
            GetInstance()->Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + GetInstance()->timeZone * SECS_PER_HOUR;
        }
    }
    Serial.println("No NTP Response :-(");
    return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void NTPManager::SendPacket(IPAddress &address)
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}