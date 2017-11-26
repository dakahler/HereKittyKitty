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
#include "InputDebounce.h"

template <class T>
class ButtonPress : public InputDebounce, public IUpdatable
{
public:
	ButtonPress(int pinNum, MethodSlot<T, const ButtonPress&> callback)
		: InputDebounce(pinNum, 50, InputDebounce::PIM_INT_PULL_UP_RES, 0),
		m_pinNum(pinNum), m_callback(callback)
	{
		
	}

	void Update() override
	{
		process(millis());
	}

protected:
	void pressed() override
	{
		m_callback(*this);
	}
	
	void released() override
	{
	  
	}

	void pressedDuration(unsigned long duration) override
	{
	  
	}

private:
	int m_pinNum;
	MethodSlot<T, const ButtonPress&> m_callback;

	bool m_lastPinState;
	bool m_unpressedPinState;
};

#endif

