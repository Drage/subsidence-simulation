
#include "MathUtils.h"
#include <math.h>
#include <limits>

const float PI = (float)acos(-1.0);

const float EPSILON = std::numeric_limits<float>::epsilon();

float DegToRad(float degrees)
{
	return degrees / 180.0f * PI;
}

float RadToDeg(float radians)
{
	return radians * 180.0f / PI;
}
