
/*
 * @file	Timer.h/.cpp
 * @brief	Stores elapsed time.
 * @author	Matt Drage
 * @date	02/01/2013
 */

#ifndef TIMER_H
#define TIMER_H

#include "Time.h"

class Timer
{
public:
	// Constructor
	Timer();
	
	// Operations
	void Start();
	void Pause();
	void Stop();
	void Reset();
	
	// Check state
	bool IsPaused() const;
	bool IsStarted() const;
	
	// Decomposition
	void ElapsedTime(int &days, int &hours, int &mins, int &secs, int &us) const;
	
	// Convert to fractional number
	double ElapsedSeconds() const;
	
	// Convert to text representation hh:mm:ss:uuuuuu
	std::string ToString();
	
private:
	Time m_startTime;
	Time m_pausedTime;
	bool m_paused;
	bool m_started;
};

#endif
