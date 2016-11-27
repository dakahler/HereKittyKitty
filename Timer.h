#ifndef _TIMER_h
#define _TIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"

class Timer : public IUpdatable
{
public:
	Timer(unsigned long intervalMs, void (*callback)(void), int numIterations = -1, bool startImmediately = true,
		void(*onStartCallback)(void) = NULL, void(*onStopCallback)(void) = NULL);
	void Update() override;
	void FireNow();
	void Stop();
	void Start();
	void Restart();
	void SetInterval(unsigned long interval);
	void SetIterations(int numIterations);
	int GetIterations()
	{
		return m_numRemainingIterations;
	}
	bool GetIsRunning()
	{
		return m_IsRunning;
	}
	unsigned long GetTimeToFire()
	{
		return millis() - m_previousMillis;
	}
	unsigned long GetInterval()
	{
		return m_intervalMs;
	}

protected:
	virtual void CallCallback();

	unsigned long m_intervalMs;
	void (*m_callback)(void);
	void(*m_onStartCallback)(void);
	void(*m_onStopCallback)(void);
	unsigned long m_previousMillis;
	int m_numRemainingIterations;
	int m_numOriginalIterations;
	bool m_IsRunning;
};

#endif

