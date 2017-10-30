
/*
 * @file	MathUtils.h/.cpp
 * @brief	Common numerical operations and constants.
 * @author	Matt Drage
 * @date	20/12/2012
 */

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

extern const float PI;
extern const float EPSILON;

float DegToRad(float degrees);

float RadToDeg(float radians);

template<typename T>
T Min(T a, T b)
{
	return (a < b) ? a : b;
}

template<typename T>
T Min(T a, ...)
{
	T result = a;
	T value;
	
	va_list args;
	va_start(args, a);
	while ((value = va_arg(args, T)) != 0)
		result = Min(result, value);
    va_end(args);

	return result;
}

template<typename T>
T Max(T a, T b)
{
	return (a > b) ? a : b;
}

template<typename T>
T Max(T a, ...)
{
	T result = a;
	T value;
	
	va_list args;
	va_start(args, a);
	while ((value = va_arg(args, T)) != 0)
		result = Max(result, value);
    va_end(args);
	
	return result;
}

template<typename T>
T Clamp(T value, T min, T max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

template<typename T>
T Wrap(T value, T min, T max)
{
	while (value < min)
		value += (max - min);
	while (value > max)
		value -= (max - min);
	return value;
}

template<typename T>
T Sign(T value)
{
	if (value < 0)
		return -1;
	else
		return 1;
}

#endif
