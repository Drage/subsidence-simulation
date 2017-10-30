
#include "Time.h"
#include <sys/time.h>
#include <sstream>
#include <iomanip>

Time::Time()
{
	m_sec = 0;
	m_usec = 0;
}

Time::Time(long secs, long usecs)
{
	while (usecs >= 1000000)
	{
        secs++;
		usecs -= 1000000;
	}

	m_sec = secs;
	m_usec = usecs;
}

Time Time::Now()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return Time(now.tv_sec, now.tv_usec);
}

Time Time::operator+ (const Time &other) const
{
	Time result;
	
	result.m_sec = m_sec + other.m_sec;
    result.m_usec = m_usec + other.m_usec;
	
    while (result.m_usec >= 1000000)
	{
        result.m_sec++;
		result.m_usec -= 1000000;
	}
	
	return result;
}

Time Time::operator- (const Time &other) const
{
	Time result;
	
	if ((m_sec < other.m_sec) || ((m_sec == other.m_sec) && (m_usec <= other.m_usec))) 
		result.m_sec = result.m_usec = 0;
	else
	{	
		result.m_sec = m_sec - other.m_sec;
		if (m_usec < other.m_usec)
		{
			result.m_usec = m_usec + 1000000 - other.m_usec;
			result.m_sec--;
		} 
		else
			result.m_usec = m_usec - other.m_usec;
	}
	
	return result;
}

void Time::operator+= (const Time &other)
{
	*this = *this + other;
}

void Time::operator-= (const Time &other)
{
	*this = *this - other;
}

void Time::operator= (const Time &other)
{
	m_sec = other.m_sec;
	m_usec = other.m_usec;
}

bool Time::operator== (const Time &other) const
{
	return m_sec == other.m_sec && m_usec == other.m_usec;
}

bool Time::operator!= (const Time &other) const
{
	return m_sec != other.m_sec || m_usec != other.m_usec;
}

bool Time::operator< (const Time &other) const
{
	if (m_sec < other.m_sec)
        return true;
    else if (m_sec > other.m_sec)
        return false;
    else if (m_usec < other.m_usec)
        return true;
    else if (m_usec > other.m_usec)
        return false;
    else
        return false;
}

bool Time::operator> (const Time &other) const
{
	if (m_sec < other.m_sec)
        return false;
    else if (m_sec > other.m_sec)
        return true;
    else if (m_usec < other.m_usec)
        return false;
    else if (m_usec > other.m_usec)
        return true;
    else
        return false;
}

void Time::Breakdown(int &days, int &hours, int &mins, int &secs, int &us) const
{
	long t = m_sec;
	
	days = t / 86400;
	t -= days * 86400;
	
	hours = t / 3600;
	t -= hours * 3600;
	
	mins = t / 60;
	t -= mins * 60;
	
	secs = t;
	
	us = (m_usec > 1000000) ? m_usec % 1000000 : m_usec;
}

double Time::ToSecs() const
{
	int usec = (m_usec > 1000000) ? m_usec % 1000000 : m_usec;
	return ((double)m_sec + (usec / 1000000.0));
}

std::string Time::ToString() const
{	
	int days, hours, mins, secs, us;
	Breakdown(days, hours, mins, secs, us);
	
	std::stringstream ss;
	
	if (days > 0)
		ss << std::setfill('0') << std::setw(2) << days << "-";
	
	ss << std::setfill('0') << std::setw(2) << hours << ":";
	ss << std::setfill('0') << std::setw(2) << mins << ":";
	ss << std::setfill('0') << std::setw(2) << secs << ".";
	ss << std::setfill('0') << std::setw(6) << us;
	return ss.str();
}
