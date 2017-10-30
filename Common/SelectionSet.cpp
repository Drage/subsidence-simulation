
#include "SelectionSet.h"
#include "MathUtils.h"

double Gaussian(double x, double m, double s)
{
	return 1 / (2 * s * sqrt(PI)) * exp(-pow((x - m), 2) / (2 * pow(s, 2)));
}

void GenerateSelectionSet(SelectionSet<int> &s, double mean, double variance, int xMin, int xMax)
{
	double stdDev = sqrt(variance);
	for (int x = xMin; x <= xMax; x++)
		s.Add(Gaussian(x, mean, stdDev), x);
}

double BivariateGaussian(double x, double y, double ma, double mb, double sa, double sb)
{
	return 1 / (2 * PI * sa * sb) * exp(-(pow(x-ma,2)/pow(sa,2) + pow(y-mb,2)/pow(sb,2)))/2;
}

void GenerateSelectionSet(SelectionSet<Vector3> &s, double mx, double mz, double vx, double vz, int r)
{
	double sx = sqrt(vx);
	double sz = sqrt(vz);
	
	for (int x = -r; x <= r; x++)
	{
		for (int z = -r; z <= r; z++)
		{
			double p = BivariateGaussian(x, z, mx, mz, sx, sz);
			s.Add(p, Vector3(x, 1, z));
		}
	}
}

void GenerateSelectionSet(SelectionSet<Vector3> &s, const Vector2 &mean, const Vector2 &variance, int r)
{
	GenerateSelectionSet(s, mean.x, mean.y, variance.x, variance.y, r);
}
