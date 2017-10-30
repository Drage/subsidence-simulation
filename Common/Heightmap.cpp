
#include "Heightmap.h"
#include "MathUtils.h"
#include "SelectionSet.h"
#include <stdexcept>
#include <cassert>

Heightmap::Heightmap()
{
	m_width = 0;
	m_depth = 0;
	m_height = NULL;
}

Heightmap::Heightmap(int width, int depth)
{
	m_width = 0;
	m_depth = 0;
	m_height = NULL;
	SetSize(width, depth);
}

Heightmap::~Heightmap()
{
	delete[] m_height;
	m_height = NULL;
}

bool Heightmap::SetSize(int width, int depth)
{
	if (m_height)
	{
		delete[] m_height;
		m_height = NULL;
	}
	
	m_width = width;
	m_depth = depth;
	
	try
	{
		m_height = new float[m_width * m_depth];
		return true;
	}
	catch (...)
	{
		return false;
	}
}

int Heightmap::GetWidth() const
{
	return m_width;
}

int Heightmap::GetDepth() const
{
	return m_depth;
}

float& Heightmap::operator()(int x, int z)
{
	assert(m_height);
	
	if (x < 0 || x >= m_width || z < 0 || z >= m_depth)
		throw std::out_of_range("Invalid heightmap index");
	
	return m_height[x * m_depth + z];
}

float Heightmap::operator()(int x, int z) const
{
	assert(m_height);
	
	if (x < 0 || x >= m_width || z < 0 || z >= m_depth)
		throw std::out_of_range("Invalid heightmap index");
	
	return m_height[x * m_depth + z];
}

void Heightmap::Smooth(float factor, bool smoothEdges)
{
	float *result = new float[m_width * m_depth];
	
	int xMin = smoothEdges ? 0 : 1;
	int zMin = smoothEdges ? 0 : 1;
	int xMax = smoothEdges ? m_width : m_width - 1;
	int zMax = smoothEdges ? m_depth : m_depth  - 1;
	
	for (int z = zMin; z < zMax; z++)
	{
		for (int x = xMin; x < xMax; x++)
		{
			float valuetotal = 0.0f;
			float weightTotal = 0.0f;
			
			for (int zOffset = -1; zOffset <= 1; zOffset++)
			{
				for (int xOffset = -1; xOffset <= 1; xOffset++)
				{
					int xIndex = x + xOffset;
					int zIndex = z + zOffset;
					if (xIndex >= 0 && xIndex < m_width && zIndex >= 0 && zIndex < m_depth)
					{
						float weight = (xIndex == x && zIndex == z) ? (1 - factor) : factor;
						weightTotal += weight;
						valuetotal += (*this)(x + xOffset, z + zOffset) * weight;
					}
				}
			}
			
			result[x * m_depth + z] = valuetotal / weightTotal;
		}
	}
	
	delete[] m_height;
	m_height = result;
}
