// ButtonPress.h

#ifndef _BUTTONPRESS_h
#define _BUTTONPRESS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"

class ButtonPress : public IUpdatable
{
public:
	ButtonPress(int pinNum, void(*callback)(void));
	void Update() override;

private:
	int m_pinNum;
	void(*m_callback)(void);

	bool m_lastPinState;
	bool m_unpressedPinState;
};

#endif

