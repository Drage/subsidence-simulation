
/*
 * @file	Random.h/.cpp
 * @brief	Pseudo-random number generation.
 * @author	Matt Drage
 * @date	05/12/2012
 */

#ifndef RANDOM_H
#define RANDOM_H

class Random
{
	public:
		static void SetSeed();
		static void SetSeed(long seed);
		static float Float();
		static float Float(float min, float max);
		static int Int(int min, int max);
		static bool Bool();
};

#endif
