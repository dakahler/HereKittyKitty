// EasyDriver.h

#ifndef _EASYDRIVER_h
#define _EASYDRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class EasyDriver
{
public:
	EasyDriver(int stpPin, int dirPin, int ms1Pin, int ms2Pin, int enPin,
		bool useMicrostepping = true);

	void EnableMotor() const;
	void DisableMotor() const;
	void SetDirection(bool forward) const;
	void Step();

private:
	void ResetPins() const;

	int m_stpPin;
	int m_dirPin;
	int m_ms1Pin;
	int m_ms2Pin;
	int m_enPin;
	bool m_useMicrostepping;

	bool m_nextStep = true;
};

#endif
