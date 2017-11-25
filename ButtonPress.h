#ifndef _BUTTONPRESS_h
#define _BUTTONPRESS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"
#include <assert.h>
#include "Callback.h"

template <class T>
class ButtonPress : public IUpdatable
{
public:
	ButtonPress(int pinNum, MethodSlot<T, const ButtonPress&> callback)
		: m_pinNum(pinNum), m_callback(callback)
	{
		pinMode(m_pinNum, INPUT_PULLUP);

		// Assume pin state at init is the "unpressed" state
		m_unpressedPinState = digitalRead(m_pinNum);
		m_lastPinState = m_unpressedPinState;
	}

	void Update() override
	{
		bool currentPinState = digitalRead(m_pinNum);
		if (currentPinState != m_lastPinState)
		{
			m_lastPinState = currentPinState;
			if (currentPinState != m_unpressedPinState)
			{
				m_callback(*this);
				delay(5);
			}
		}
	}

private:
	int m_pinNum;
	MethodSlot<T, const ButtonPress&> m_callback;

	bool m_lastPinState;
	bool m_unpressedPinState;
};

#endif

