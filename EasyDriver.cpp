#include "EasyDriver.h"

EasyDriver::EasyDriver(int stepPin, int directionPin, int ms1Pin, int ms2Pin, int enabledPin,
	bool useMicrostepping = true)
	: m_stepPin(stepPin), m_directionPin(directionPin), m_ms1Pin(ms1Pin), m_ms2Pin(ms2Pin),
	  m_enabledPin(enabledPin), m_useMicrostepping(useMicrostepping)
{
	pinMode(m_stepPin, OUTPUT);
	pinMode(m_directionPin, OUTPUT);
	pinMode(m_ms1Pin, OUTPUT);
	pinMode(m_ms2Pin, OUTPUT);
	pinMode(m_enabledPin, OUTPUT);

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
	digitalWrite(m_stepPin, LOW);
	digitalWrite(m_directionPin, LOW);
	digitalWrite(m_ms1Pin, LOW);
	digitalWrite(m_ms2Pin, LOW);
	digitalWrite(m_enabledPin, HIGH);
}

void EasyDriver::EnableMotor() const
{
	digitalWrite(m_enabledPin, LOW);
}

void EasyDriver::DisableMotor() const
{
	digitalWrite(m_enabledPin, HIGH);
}

void EasyDriver::SetDirection(bool forward) const
{
	digitalWrite(m_directionPin, forward ? LOW : HIGH);
}

void EasyDriver::Step()
{
	digitalWrite(m_stepPin, m_nextStep ? HIGH : LOW);
	m_nextStep = !m_nextStep;
}
