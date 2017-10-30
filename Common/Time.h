
/*
 * @file	Time.h/.cpp
 * @brief	Stores a time value with microsecond precision.
 * @author	Matt Drage
 * @date	02/01/2013
 */

#ifndef TIME_H
#define TIME_H

#include <string>

class Time
{
	public:
		// Constructors
		Time();
		Time(long secs, long usecs = 0);
	
		// Get the current time
		static Time Now();
	
		// Operators
		Time operator+ (const Time &other) const;
		Time operator- (const Time &other) const;
		void operator+= (const Time &other);
		void operator-= (const Time &other);
		void operator= (const Time &other);
		bool operator== (const Time &other) const;
		bool operator!= (const Time &other) const;
		bool operator< (const Time &other) const;
		bool operator> (const Time &other) const;
	
		// Decomposition
		void Breakdown(int &days, int &hours, int &mins, int &secs, int &us) const;
	
		// Conversion to fractional number
		double ToSecs() const;
	
		// Conversion to text representation hh:mm:ss:uuuuuu
		std::string ToString() const;

	private:
		long m_sec;
		long m_usec;
};

#endif
