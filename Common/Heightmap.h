
/*
 * @file	Heightmap.h/.cpp
 * @brief	Stores surface elevation data.
 * @details	Smoothing factor should be in range [0, 1].
 * @author	Matt Drage
 * @date	20/12/2012
 */

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

class Heightmap
{
	public:
		// Constructors & Destructors
		Heightmap();
		Heightmap(int width, int depth);
		virtual ~Heightmap();

		// Dimensions
		bool SetSize(int width, int depth);
		int GetWidth() const;
		int GetDepth() const;
	
		// Height array access
		float& operator()(int x, int z);
		float operator()(int x, int z) const;
	
		// Smoothing via weighted average
		void Smooth(float factor, bool smoothEdges = true);

	private:
		int m_width;
		int m_depth;
		float *m_height;
};

#endif
