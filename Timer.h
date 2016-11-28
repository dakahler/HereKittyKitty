#ifndef _TIMER_h
#define _TIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IUpdatable.h"
#include "Callback.h"
#include <assert.h>

template <class T>
class Timer : public IUpdatable
{
public:
	Timer(unsigned long intervalMs, MethodSlot<T, const Timer&> callback, int numIterations = -1, bool startImmediately = true,
		MethodSlot<T, const Timer&> onStartCallback = MethodSlot<T, const Timer&>(nullptr, nullptr),
		MethodSlot<T, const Timer&> onStopCallback = MethodSlot<T, const Timer&>(nullptr, nullptr))
		: m_intervalMs(intervalMs), m_callback(callback), m_onStartCallback(onStartCallback), m_onStopCallback(onStopCallback),
		m_numRemainingIterations(numIterations), m_numOriginalIterations(numIterations), m_IsRunning(startImmediately)
	{
		assert(m_intervalMs > 0);

		if (startImmediately)
		{
			Start();
		}
	}

	void Update() override
	{
		if (!m_IsRunning || m_numRemainingIterations == 0)
			return;

		if (millis() - m_previousMillis >= m_intervalMs)
		{
			FireNow();
		}
	}

	void FireNow()
	{
		m_previousMillis = millis();

		if (m_numRemainingIterations > 0)
		{
			m_numRemainingIterations--;
			if (m_numRemainingIterations == 0)
			{
				Stop();
			}
		}

		CallCallback();
	}

	void Stop()
	{
		m_IsRunning = false;
		m_onStopCallback(*this);
	}

	void Start()
	{
		m_previousMillis = millis();
		m_IsRunning = true;
		m_onStartCallback(*this);
	}

	void Restart()
	{
		m_numRemainingIterations = m_numOriginalIterations;
		Start();
	}

	void SetInterval(unsigned long interval)
	{
		m_intervalMs = interval;
	}

	void SetIterations(int numIterations)
	{
		m_numOriginalIterations = numIterations;
	}

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
	virtual void CallCallback()
	{
		m_callback(*this);
	}

	unsigned long m_intervalMs;
	MethodSlot<T, const Timer&> m_callback;
	MethodSlot<T, const Timer&> m_onStartCallback;
	MethodSlot<T, const Timer&> m_onStopCallback;
	unsigned long m_previousMillis;
	int m_numRemainingIterations;
	int m_numOriginalIterations;
	bool m_IsRunning;
};

#endif

