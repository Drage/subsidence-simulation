
/*
 * @file	CellGrid3D.h/.cpp
 * @brief	Stores 3D cellular automata data.
 * @details	Each cell in the grid is stored as a single byte - 255 possible cell types.
 *			SetSize(w, h, d) must be called before the CellGrid can be used.
 *			BoundMode determines behaviour in situations when cell coordinates are not
 *			within the bounds of the grid. Each border may be set independantly.
 *				- Border: all cells outside the border are assumed to be a particular 
 *					value. Set via the 'option' parameter in SetBoundMode().
 *				- Wrap: coordinates outside the border wrap around to the opposite
 *					border. Can be used to fake grid of infinite size.
 *				- Exception: any coordinate outside the border will throw an exception,
 *					if this is not caught the program will abort.
 *				- Ignore: all coordinates outside the border will be ignored. Get will
 *					return an undefined value. Set will have no effect.
 * @author	Matt Drage
 * @date	27/12/2012
 */

#ifndef CELLGRID3D_H
#define CELLGRID3D_H

#include "Vector3.h"
#include <string>

class CellGrid3D
{
	public:
		enum BoundMode { BORDER, WRAP, EXCEPTION, IGNORE };
		enum Border { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, FRONT = 4, BACK = 5 };
	
		// Constructors & Destructors
		CellGrid3D();
		CellGrid3D(int width, int height, int depth);
		CellGrid3D(const Vector3 &size);
		virtual ~CellGrid3D();

		// Dimensions
		bool SetSize(int width, int height, int depth);
		bool SetSize(const Vector3 &size);
		int GetWidth() const;
		int GetHeight() const;
		int GetDepth() const;
	
		// Bound modes
		void SetBoundMode(enum BoundMode mode, char option = 0);
		void SetBoundMode(enum BoundMode mode, enum Border border, char option = 0);
		void SetBoundMode(const std::string &mode, enum Border border);
		enum BoundMode GetBoundMode(enum Border border) const;
	
		// Cell access
		char& operator()(int x, int y, int z);
		char operator()(int x, int y, int z) const;
		char& operator()(const Vector3 &position);
		char operator()(const Vector3 &position) const;
	
		// Cell value initialisation
		void Fill(char value);
		void Fill(int x, int y, int z, int width, int height, int depth, char value);
		void Fill(const Vector3 &position, const Vector3 &dimensions, char value);
	
		// Raw data access
		char* GetRow(int index);
		char* GetRawData();
	
		// Copy cell data
		void CopyCells(char *cellData, int w, int h, int d, int x, int y, int z);
		void CopyCells(char *cellData, const Vector3 &dimensions, const Vector3 &position);
	
	private:
		void InitBorders();
		bool ApplyBounds(int &x, int &y, int &z) const;
		int ConvertIndex(int x, int y, int z) const;
	
		int m_width;
		int m_height;
		int m_depth;
		char m_border[6];
		enum BoundMode m_boundMode[6];
		char *m_cells;
		char m_dummy;
};

#endif
