
#include "Timer.h"

Timer::Timer()
{
	m_paused = false;
	m_started = false;
}

void Timer::Start()
{
	if (m_paused)
    {
        m_paused = false;
        m_startTime = Time::Now() - m_pausedTime;
    }
	else
	{
		m_started = true;
		m_startTime = Time::Now();
	}
}

void Timer::Stop()
{
	m_started = false;
	m_paused = false;
}

void Timer::Reset()
{
	m_started = true;
	m_paused = false;
	m_startTime = Time::Now();
}

void Timer::Pause()
{
	if (m_started && !m_paused)
    {
        m_paused = true;
        m_pausedTime = Time::Now() - m_startTime;
    }
}

bool Timer::IsPaused() const
{
	return m_paused;
}

bool Timer::IsStarted() const
{
	return m_started;
}

double Timer::ElapsedSeconds() const
{
	if (m_started)
    {
        if (m_paused)
            return m_pausedTime.ToSecs();
        else
            return (Time::Now() - m_startTime).ToSecs();
    }
    return 0;
}

void Timer::ElapsedTime(int &days, int &hours, int &mins, int &secs, int &us) const
{
	if (m_started)
    {
        if (m_paused)
            m_pausedTime.Breakdown(days, hours, mins, secs, us);
        else
            (Time::Now() - m_startTime).Breakdown(days, hours, mins, secs, us);
    }
	else 
		days = hours = mins = secs = us = 0;
}

std::string Timer::ToString()
{
	if (m_started)
    {
        if (m_paused)
            return m_pausedTime.ToString();
        else
            return (Time::Now() - m_startTime).ToString();
    }
	return "00:00:00.000000";
}

