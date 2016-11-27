#include "Timer.h"
#include <assert.h>

Timer::Timer(unsigned long intervalMs, void(*callback)(void), int numIterations, bool startImmediately,
	void(*onStartCallback)(void), void(*onStopCallback)(void))
	: m_intervalMs(intervalMs), m_callback(callback), m_onStartCallback(onStartCallback), m_onStopCallback(onStopCallback),
	  m_numRemainingIterations(numIterations), m_numOriginalIterations(numIterations), m_IsRunning(startImmediately)
{
	assert(m_intervalMs > 0);

	if (startImmediately)
	{
		Start();
	}
}

void Timer::Update()
{
	if (!m_IsRunning || m_numRemainingIterations == 0)
		return;

	if (millis() - m_previousMillis >= m_intervalMs)
	{
		FireNow();
	}
}

void Timer::FireNow()
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

void Timer::Stop()
{
	m_IsRunning = false;

	if (m_onStopCallback != NULL)
	{
		m_onStopCallback();
	}
}

void Timer::Start()
{
	m_previousMillis = millis();
	m_IsRunning = true;

	if (m_onStartCallback != NULL)
	{
		m_onStartCallback();
	}
}

void Timer::Restart()
{
	m_numRemainingIterations = m_numOriginalIterations;
	Start();
}

void Timer::SetInterval(unsigned long interval)
{
	m_intervalMs = interval;
}

void Timer::SetIterations(int numIterations)
{
	m_numOriginalIterations = numIterations;
}

void Timer::CallCallback()
{
	if (m_callback != NULL)
	{
		m_callback();
	}
}
