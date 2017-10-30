
#include "CellGrid2D.h"
#include "MathUtils.h"
#include <stdexcept>
#include <cassert>

CellGrid2D::CellGrid2D()
{
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	
	for (int i = 0; i < 4; i++)
	{
		m_boundMode[i] = WRAP;
		m_border[i] = 0;
	}
}

CellGrid2D::CellGrid2D(int width, int height)
{
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	
	for (int i = 0; i < 4; i++)
	{
		m_boundMode[i] = WRAP;
		m_border[i] = 0;
	}
	
	SetSize(width, height);
}

CellGrid2D::~CellGrid2D()
{
	delete[] m_cells;
	m_cells = NULL;
}

bool CellGrid2D::SetSize(int width, int height)
{
	if (m_cells)
	{
		delete[] m_cells;
		m_cells = NULL;
	}
	
	m_width = width;
	m_height = height;
	
	try
	{
		m_cells = new char[m_width * m_height];
		return true;
	}
	catch (...)
	{
		return false;
	}
}

int CellGrid2D::GetWidth() const
{
	return m_width;
}

int CellGrid2D::GetHeight() const
{
	return m_height;
}

void CellGrid2D::SetBoundMode(enum BoundMode mode, char option)
{
	for (int i = 0; i < 4; i++)
		m_boundMode[i] = mode;
	
	if (mode == BORDER)
	{
		for (int i = 0; i < 4; i++)
			m_border[i] = option;
	}
}

void CellGrid2D::SetBoundMode(enum BoundMode mode, enum Border border, char option)
{
	m_boundMode[border] = mode;
	if (mode == BORDER)
		m_border[border] = option;
}

enum CellGrid2D::BoundMode CellGrid2D::GetBoundMode(enum Border border) const
{
	return m_boundMode[border];
}

bool CellGrid2D::ApplyBounds(int &x, int &y) const
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
	return false;
}

char& CellGrid2D::operator()(int x, int y)
{
	bool ignore = ApplyBounds(x, y);
	if (ignore) return m_dummy;
	return m_cells[y * m_width + x];
}

char CellGrid2D::operator()(int x, int y) const
{
	bool ignore = ApplyBounds(x, y);
	if (ignore) return m_dummy;
	return m_cells[y * m_width + x];
}

void CellGrid2D::Fill(char value)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
			m_cells[y * m_width + x] = value;
	}
}

void CellGrid2D::FillRect(int x, int y, int width, int height, char value)
{
	for (int cy = y; cy < y + height; cy++)
	{
		for (int cx = x; cx < x + width; cx++)
			(*this)(cx, cy) = value;
	}
}

char* CellGrid2D::GetRow(int index)
{
	return m_cells + index * m_width;
}

char* CellGrid2D::GetRawData()
{
	return m_cells;
}

void CellGrid2D::CopyCells(char *cellData, int width, int height, int xPosition, int yPosition)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
			m_cells[(y + yPosition) * m_width + (x + xPosition)] = cellData[y * width + x];
	}
}
