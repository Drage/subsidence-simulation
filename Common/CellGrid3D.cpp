
#include "CellGrid3D.h"
#include "MathUtils.h"
#include <stdexcept>
#include <cassert>
#include <cmath>

CellGrid3D::CellGrid3D()
{
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	InitBorders();
}

CellGrid3D::CellGrid3D(int width, int height, int depth)
{
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	InitBorders();
	SetSize(width, height, depth);
}

CellGrid3D::CellGrid3D(const Vector3 &size)
{
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	InitBorders();
	SetSize(size);
}

CellGrid3D::~CellGrid3D()
{
	delete[] m_cells;
	m_cells = NULL;
}

void CellGrid3D::InitBorders()
{
	for (int i = 0; i < 6; i++)
	{
		m_boundMode[i] = WRAP;
		m_border[i] = 0;
	}
}

bool CellGrid3D::SetSize(int width, int height, int depth)
{
	if (m_cells)
	{
		delete[] m_cells;
		m_cells = NULL;
	}
	
	m_width = width;
	m_height = height;
	m_depth = depth;
	
	try
	{
		m_cells = new char[m_width * m_height * m_depth];
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool CellGrid3D::SetSize(const Vector3 &size)
{
	return SetSize(size.x, size.y, size.z);
}

int CellGrid3D::GetWidth() const
{
	return m_width;
}

int CellGrid3D::GetHeight() const
{
	return m_height;
}

int CellGrid3D::GetDepth() const
{
	return m_depth;
}

void CellGrid3D::SetBoundMode(enum BoundMode mode, char option)
{
	for (int i = 0; i < 6; i++)
		m_boundMode[i] = mode;
	
	if (mode == BORDER)
	{
		for (int i = 0; i < 6; i++)
			m_border[i] = option;
	}
}

void CellGrid3D::SetBoundMode(enum BoundMode mode, enum Border border, char option)
{
	m_boundMode[border] = mode;
	if (mode == BORDER)
		m_border[border] = option;
}

void CellGrid3D::SetBoundMode(const std::string &mode, enum Border border)
{
	if (mode == "exception")
		m_boundMode[border] = EXCEPTION;
	else if (mode == "ignore")
		m_boundMode[border] = IGNORE;
	else if (mode == "wrap")
		m_boundMode[border] = WRAP;
	else if (mode.substr(0, 6) == "border")
	{
		m_boundMode[border] = BORDER;
		m_border[border] = mode[7];
	}
}

enum CellGrid3D::BoundMode CellGrid3D::GetBoundMode(enum Border border) const
{
	return m_boundMode[border];
}

bool CellGrid3D::ApplyBounds(int &x, int &y, int &z) const
{
	if (x >= m_width || x < 0)
	{
		int b = x < 0 ? LEFT : RIGHT;
		if (m_boundMode[b] == BORDER)
			return m_border[b];
		else if (m_boundMode[b] == WRAP)
			x = Wrap(x, 1, m_width - 1);
		else if (m_boundMode[b] == EXCEPTION)
			throw std::out_of_range("Invalid cell index");
		else
			return true;
	}
	if (y >= m_height || y < 0)
	{
		int b = y < 0 ? BOTTOM : TOP;
		if (m_boundMode[b] == BORDER)
			return m_border[b];
		else if (m_boundMode[b] == WRAP)
			y = Wrap(y, 1, m_height - 1);
		else if (m_boundMode[b] == EXCEPTION)
			throw std::out_of_range("Invalid cell index");
		else
			return true;
	}
	if (z >= m_depth || z < 0)
	{
		int b = z < 0 ? FRONT : BACK;
		if (m_boundMode[b] == BORDER)
			return m_border[b];
		else if (m_boundMode[b] == WRAP)
			z = Wrap(z, 1, m_depth - 1);
		else if (m_boundMode[b] == EXCEPTION)
			throw std::out_of_range("Invalid cell index");
		else
			return true;
	}
	return false;
}

int CellGrid3D::ConvertIndex(int x, int y, int z) const
{
	return y * m_width * m_depth + x * m_depth + z;
}

char& CellGrid3D::operator()(int x, int y, int z)
{
	bool ignore = ApplyBounds(x, y, z);
	if (ignore) return m_dummy;
	return m_cells[ConvertIndex(x, y, z)];
}

char CellGrid3D::operator()(int x, int y, int z) const
{
	bool ignore = ApplyBounds(x, y, z);
	if (ignore) return m_dummy;
	return m_cells[ConvertIndex(x, y, z)];
}

char& CellGrid3D::operator()(const Vector3 &position)
{
	return (*this)(position.x, position.y, position.z);
}

char CellGrid3D::operator()(const Vector3 &position) const
{
	return (*this)(position.x, position.y, position.z);
}

void CellGrid3D::Fill(char value)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			for (int z = 0; z < m_depth; z++)
				m_cells[ConvertIndex(x, y, z)] = value;
		}
	}
}

void CellGrid3D::Fill(int x, int y, int z, int width, int height, int depth, char value)
{
	for (int cy = y; cy < y + height; cy++)
	{
		for (int cx = x; cx < x + width; cx++)
		{
			for (int cz = z; cz < z + depth; cz++)
				(*this)(cx, cy, cz) = value;
		}
	}
}

void CellGrid3D::Fill(const Vector3 &position, const Vector3 &dimensions, char value)
{
	Fill(position.x, position.y, position.z, ceil(dimensions.x), ceil(dimensions.y), ceil(dimensions.z), value);
}

char* CellGrid3D::GetRow(int index)
{
	return m_cells + index * m_width * m_depth;
}

char* CellGrid3D::GetRawData()
{
	return m_cells;
}

void CellGrid3D::CopyCells(char *cellData, int w, int h, int d, int x, int y, int z)
{
	for (int yy = 0; yy < h; yy++)
	{
		for (int xx = 0; xx < w; xx++)
		{
			for (int zz = 0; zz < d; zz++)
				m_cells[ConvertIndex(xx + x, yy + y, zz + z)] = cellData[ConvertIndex(xx, yy, zz)];
		}
	}
}

void CellGrid3D::CopyCells(char *cellData, const Vector3 &dimensions, const Vector3 &position)
{
	CopyCells(cellData, dimensions.x, dimensions.y, dimensions.z, position.x, position.y, position.z);
}
