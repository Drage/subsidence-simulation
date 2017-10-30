
/*
 * @file	Png.h/.cpp
 * @brief	Loads PNG image data
 * @details	Supports true-colour, palette colour, greyscale, and alpha channel.
 *			Only supports 8-bit depth. I.e. 24bit RGB, 32bit RGBA, 8bit greyscale, and 256-colour palette images.
 *			Does not support interlacing.
 *			Uses zLib for decompression of pixel data.
 *			File Specification: http://www.w3.org/TR/PNG/
 * @author	Matt Drage
 * @date	22/01/2013
 */

#ifndef PNG_H
#define PNG_H

#include <string>
#include "Colour.h"

class Png
{
	public:
		// Constructors
		Png();
		Png(const std::string &filename);
		virtual ~Png();

		// Load file
		bool Load(const std::string &filename);
	
		// Image properties
		unsigned GetWidth() const;
		unsigned GetHeight() const;
		unsigned GetBPP() const;
		unsigned long GetSize() const;
	
		// Pixel data access
		unsigned char* GetPixels() const;
		Colour GetPixel(unsigned x, unsigned y) const;

	private:
		static const int signature[8];
		enum ColourType { GREYSCALE = 0, INDEXED = 3, TRUECOLOUR = 2, ALPHA = 4 };
		enum InterlaceMethod { NO_INTERLACE = 0, ADAM7 = 1 };
		enum Filter { NO_FILTER = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4 };

		bool IsPNG(const unsigned char* data);
		void GetChunkInfo(unsigned char* chunk, unsigned &length, char type[], unsigned char* &data, unsigned &crc);
		bool ReadIHDR(unsigned char* data);
		void ReadPLTE(unsigned char* data);
		void ReadIDAT(unsigned char* data, unsigned length);

		bool Decompress();
		
		void ApplyFiltering();
		void FilterSub(unsigned char *scanline);
		void FilterUp(unsigned char *scanline, unsigned char *previous);
		void FilterAverage(unsigned char *scanline, unsigned char *previous);
		void FilterPaeth(unsigned char *scanline, unsigned char *previous);

		void ApplyPalette();
		void FlipRowOrder();
	
		unsigned m_width;
		unsigned m_height;
		unsigned m_bpp;
		unsigned char *m_pixels;
		unsigned long m_imageSize;

		bool m_readIHDR;
		unsigned char m_bitDepth;
		unsigned char m_colourType;
		unsigned char m_compression;
		unsigned char m_filter;
		unsigned char m_interlace;

		bool m_readPLTE;
		unsigned char *m_palette;
		unsigned char *m_pixelIndexes;

		unsigned char *m_dataStartPos;
		unsigned char *m_dataConcatenatePos;
};

#endif
