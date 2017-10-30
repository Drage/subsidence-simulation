
#include "Random.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <cassert>

void Random::SetSeed()
{
	srand((long)time(NULL));
}

void Random::SetSeed(long seed)
{
	srand(seed);
}

float Random::Float()
{
	return rand() / (float)RAND_MAX;
}

float Random::Float(float min, float max)
{
	return (rand() / (float)RAND_MAX) * (max - min) + min;
}

int Random::Int(int min, int max)
{
	return rand() % (max - min) + min;
}

bool Random::Bool()
{
	return rand() % 2 == 0;
}
