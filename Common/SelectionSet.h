
/*
 * @file	SelectionSet.h/.cpp
 * @brief	Random roulette selection from a set a values.
 * @details	Selects values using roulette-wheel style selection (uses cumulative probability).
 *			Probability of all items added does not have to equal 1.
 *			2D and 3D sets can be generated using gaussian probability distributions.
 * @author	Matt Drage
 * @date	05/12/2012
 */

#ifndef SELECTIONSET_H
#define SELECTIONSET_H

#include <vector>
#include "Random.h"
#include "Vector2.h"
#include "Vector3.h"

template <typename T>
class SelectionSet
{
	public:
		SelectionSet()
		{
			m_setSize = 0;
			m_total = 0;
		}
		
		void Clear()
		{
			m_setSize = 0;
			m_total = 0;
			m_cumulativeProb.clear();
			m_values.clear();
		}
		
		int GetSetSize() const
		{
			return m_setSize;
		}
		
		void Add(double probability, T value)
		{
			m_setSize++;
			m_total += probability;
			m_cumulativeProb.push_back(m_total);
			m_values.push_back(value);
		}
		
		T RouletteSelect()
		{
			float r = Random::Float(0, m_total);
			for (int i = 0; i < m_setSize; i++)
			{
				if (r < m_cumulativeProb[i])
					return m_values[i];
			}
			return m_values[m_setSize - 1];
		}
		
	private:
		int m_setSize;
		float m_total;
		std::vector<double> m_cumulativeProb;
		std::vector<T> m_values;
};

// Populate selection set using Guassian distribution
double Gaussian(double x, double m, double s);
double BivariateGaussian(double x, double y, double ma, double mb, double sa, double sb);
void GenerateSelectionSet(SelectionSet<int> &s, double mean, double variance, int xMin, int xMax);
void GenerateSelectionSet(SelectionSet<Vector3> &s, double mx, double mz, double vx, double vz, int r);
void GenerateSelectionSet(SelectionSet<Vector3> &s, const Vector2 &mean, const Vector2 &variance, int r);

#endif
