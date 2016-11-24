#include "EasyDriver.h"

EasyDriver::EasyDriver(int stpPin, int dirPin, int ms1Pin, int ms2Pin, int enPin,
	bool useMicrostepping = true)
	: m_stpPin(stpPin), m_dirPin(dirPin), m_ms1Pin(ms1Pin), m_ms2Pin(ms2Pin),
	  m_enPin(enPin), m_useMicrostepping(useMicrostepping)
{
	pinMode(m_stpPin, OUTPUT);
	pinMode(m_dirPin, OUTPUT);
	pinMode(m_ms1Pin, OUTPUT);
	pinMode(m_ms2Pin, OUTPUT);
	pinMode(m_enPin, OUTPUT);

	ResetPins();

	if (m_useMicrostepping)
	{
		// Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
		digitalWrite(m_ms1Pin, HIGH);
		digitalWrite(m_ms2Pin, HIGH);
	}
	else
	{
		digitalWrite(m_ms1Pin, LOW);
		digitalWrite(m_ms2Pin, LOW);
	}
}

void EasyDriver::ResetPins() const
{
	digitalWrite(m_stpPin, LOW);
	digitalWrite(m_dirPin, LOW);
	digitalWrite(m_ms1Pin, LOW);
	digitalWrite(m_ms2Pin, LOW);
	digitalWrite(m_enPin, HIGH);
}

void EasyDriver::EnableMotor() const
{
	digitalWrite(m_enPin, LOW);
}

void EasyDriver::DisableMotor() const
{
	digitalWrite(m_enPin, HIGH);
}

void EasyDriver::SetDirection(bool forward) const
{
	digitalWrite(m_dirPin, forward ? LOW : HIGH);
}

void EasyDriver::Step()
{
	digitalWrite(m_stpPin, m_nextStep ? HIGH : LOW);
	m_nextStep = !m_nextStep;
}
