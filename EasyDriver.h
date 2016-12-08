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
	EasyDriver(int stepPin, int directionPin, int ms1Pin, int ms2Pin, int ms3Pin, int enabledPin,
		bool useMicrostepping = true);

	void EnableMotor() const;
	void DisableMotor() const;
	void SetDirection(bool forward) const;
	void Step();

private:
	void ResetPins() const;

	int m_stepPin;
	int m_directionPin;
	int m_ms1Pin;
	int m_ms2Pin;
	int m_ms3Pin;
	int m_enabledPin;
	bool m_useMicrostepping;

	bool m_nextStep = true;
};

#endif

