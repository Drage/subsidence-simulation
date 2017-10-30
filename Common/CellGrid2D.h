
#ifndef CELLGRID2D_H
#define CELLGRID2D_H

/*
 * @file	CellGrid2D.h/.cpp
 * @brief	Stores 2D cellular automata data.
 * @details	Each cell in the grid is stored as a single byte - 255 possible cell types.
 *			Data is represented internally as a row-major matrix of bytes.
 *			SetSize(w, h) must be called before the CellGrid can be used.
 *			BoundMode determines behaviour in situations when cell coordinates are not
 *			within the bounds of the grid (x < 0 or >= width, y < 0 or >= height).
 *			The BoundMode for each border may be set independantly.
 *				- Border: all cells outside the border are assumed to be a particular 
 *					value. Set via the 'option' parameter in SetBoundMode().
 *				- Wrap: coordinates outside the border wrap around to the opposite
 *					border. Can be used to fake grid of infinite size.
 *				- Exception: any coordinate outside the border will throw an exception,
 *					if this is not caught the program will abort.
 *				- Ignore: all coordinates outside the border will be ignored. Get will
 *					return an undefined value. Set will have no effect.
 * @author	Matt Drage
 * @date	12/12/2012
 */

class CellGrid2D
{
	public:
		enum BoundMode { BORDER, WRAP, EXCEPTION, IGNORE };
		enum Border { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3 };
	
		// Constructors & Destructors
		CellGrid2D();
		CellGrid2D(int width, int height);
		virtual ~CellGrid2D();

		// Dimensions
		bool SetSize(int width, int height);
		int GetWidth() const;
		int GetHeight() const;
	
		// Bound modes
		void SetBoundMode(enum BoundMode mode, char option = 0);
		void SetBoundMode(enum BoundMode mode, enum Border border, char option = 0);
		enum BoundMode GetBoundMode(enum Border border) const;
	
		// Cell access
		char& operator()(int x, int y);
		char operator()(int x, int y) const;
	
		// Cell value initialisation
		void Fill(char value);
		void FillRect(int x, int y, int width, int height, char value);
	
		// CellGrid data copying (used for reconstructing larger CellGrid from multiple smaller
		// ones when using MPI to divide grid).
		void CopyCells(char *cellData, int width, int height, int xPosition, int yPosition);
	
		// Raw data access
		char* GetRow(int index);
		char* GetRawData();

	private:
		bool ApplyBounds(int &x, int &y) const;
	
		int m_width;
		int m_height;
		char m_border[4];
		enum BoundMode m_boundMode[4];
		char *m_cells;
		char m_dummy;
};

#endif
