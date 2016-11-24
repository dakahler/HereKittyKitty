#include "ButtonPress.h"
#include <assert.h>

ButtonPress::ButtonPress(int pinNum, void(*callback)(void))
	: m_pinNum(pinNum), m_callback(callback)
{
	assert(m_callback != NULL);

	pinMode(m_pinNum, INPUT_PULLUP);

	// Assume pin state at init is the "unpressed" state
	m_unpressedPinState = digitalRead(m_pinNum);
	m_lastPinState = m_unpressedPinState;
}

void ButtonPress::Update()
{
	bool currentPinState = digitalRead(m_pinNum);
	if (currentPinState != m_lastPinState)
	{
		if (currentPinState != m_unpressedPinState)
		{
			m_callback();
		}
	}
}
