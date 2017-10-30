
#include "Png.h"
#include <fstream>
#include <zlib.h>
#include "MathUtils.h"
#include <string.h>

const int Png::signature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

template<typename T, int size>
int ArrayLength(T(&)[size]) { return size; }

Png::Png()
{
	m_width = 0;
	m_height = 0;
	m_bpp = 0;
	m_imageSize = 0;
	m_pixels = NULL;
	m_readIHDR = false;
	m_readPLTE = false;
	m_bitDepth = 0;
	m_colourType = 0;
	m_compression = 0;
	m_filter = 0;
	m_interlace = 0;
	m_dataStartPos = NULL;
}

Png::Png(const std::string &filename)
{
	m_width = 0;
	m_height = 0;
	m_bpp = 0;
	m_imageSize = 0;
	m_pixels = NULL;
	m_readIHDR = false;
	m_readPLTE = false;
	m_bitDepth = 0;
	m_colourType = 0;
	m_compression = 0;
	m_filter = 0;
	m_interlace = 0;
	m_dataStartPos = NULL;
	Load(filename);
}

Png::~Png()
{
	if (m_pixels)
	{
		delete[] m_pixels;
		m_pixels = NULL;
	}
}

bool Png::Load(const std::string &filename)
{
	unsigned long fileSize;
	unsigned char* fileData = NULL;
	std::ifstream file;
 
	try
	{
		// Open the file
		file.open(filename.c_str(), std::ios::binary);
 
		if (!file.is_open())
			throw std::exception();
 
		// Get file size
		file.seekg(0, std::ios_base::end);
		fileSize = (long)file.tellg();
		file.seekg(0, std::ios_base::beg);
 
		// Allocate memory
		fileData = new unsigned char[fileSize];
 
		if (fileData == NULL)
			throw std::exception();
 
		// Read the file into memory
		file.read((char*)fileData, fileSize);
		file.close();

		// Check file is PNG
		if (!IsPNG(fileData))
			throw std::exception();

		// Read data
		unsigned char* chunk = fileData + ArrayLength(signature);
		unsigned length;
		char typeCode[4];
		unsigned char* chunkData;
		unsigned crc;
		bool fileEnd = false;
	
		while (!fileEnd)
		{
			GetChunkInfo(chunk, length, typeCode, chunkData, crc);

			if (memcmp(typeCode, "IHDR", 4) == 0)
			{
				// Read header information
				if (!ReadIHDR(chunkData))
					throw std::exception();
			}
			else if (memcmp(typeCode, "PLTE", 4) == 0)
			{
				// Copy palette information
				ReadPLTE(chunkData);
			}
			else if (memcmp(typeCode, "IDAT", 4) == 0)
			{
				// Concatenate data - prepare for decompression
				ReadIDAT(chunkData, length);
			}
			else if (memcmp(typeCode, "IEND", 4) == 0)
			{
				fileEnd = true;
			}

			chunk += length + 12;
		}

		// Apply zLib inflate to compressed data
		if (!Decompress())
			throw std::exception();

		// Reconstruct image from palette
		if (m_colourType == INDEXED)
			ApplyPalette(); 

		// Reconstruct image from scanline filtering
		ApplyFiltering(); 

		// Flip row order so image is right way up
		FlipRowOrder();

		delete[] fileData; 
		file.close();
		return true;
	}
	catch (std::exception&)
	{
		if (file.is_open())
			file.close();

		if (fileData)
			delete[] fileData;

		return false;
	}
}

bool Png::IsPNG(const unsigned char* data)
{
	try
	{
		for (int i = 0; i < ArrayLength(signature); i++)
		{
			if (data[i] != signature[i])
				return false;
		}
		return true;
	}
	catch (std::exception&)
	{
		return false;
	}
}

void Png::GetChunkInfo(unsigned char* chunk, unsigned &length, char type[], unsigned char* &data, unsigned &crc)
{
	length = chunk[0];
	length = (length << 8) + chunk[1];
	length = (length << 8) + chunk[2];
	length = (length << 8) + chunk[3];

	memcpy(type, &chunk[4], 4);

	data = chunk + 8;

	if (memcmp(type, "IEND", 4) != 0)
		memcpy(&crc, &chunk[length + 8], 4);
}

bool Png::ReadIHDR(unsigned char* data)
{
	// Get image dimensions
	m_width = data[0];
	m_width = (m_width << 8) + data[1];
	m_width = (m_width << 8) + data[2];
	m_width = (m_width << 8) + data[3];

	m_height = data[4];
	m_height = (m_height << 8) + data[5];
	m_height = (m_height << 8) + data[6];
	m_height = (m_height << 8) + data[7];

	// Get other metadata
	m_bitDepth = data[8];
	m_colourType = data[9];
	m_compression = data[10];
	m_filter = data[11];
	m_interlace = data[12];

	// Don't load unsupported file formats
	if (m_colourType == GREYSCALE + ALPHA || m_bitDepth != 8 || m_interlace != NO_INTERLACE || m_compression != 0 || m_filter != 0)
		return false;

	// Calculate number of bits-per-pixel
	switch (m_colourType)
	{
		case GREYSCALE:
			m_bpp = m_bitDepth;
			break;
		case TRUECOLOUR:
			m_bpp = 3 * m_bitDepth;
			break;
		case INDEXED:
			m_bpp = 24;
			break;
		case (GREYSCALE + ALPHA):
			m_bpp = 2 * m_bitDepth;
			break;
		case (TRUECOLOUR + ALPHA):
			m_bpp = 4 * m_bitDepth;
			break;
	}

	// Allocate memory for pixel data: image size * bytes-per-pixel + one extra byte per scanline for filter type
	m_imageSize = (m_width * m_height * m_bpp/8) + m_height;
	m_pixels = new unsigned char[m_imageSize];

	m_readIHDR = true;

	return m_pixels != NULL;
}

void Png::ReadPLTE(unsigned char* data)
{
	m_palette = data;
	m_readPLTE = true;
}

void Png::ReadIDAT(unsigned char* data, unsigned length)
{
	if (m_dataStartPos == NULL)
	{
		m_dataStartPos = data;
		m_dataConcatenatePos = m_dataStartPos + length;
	}
	else
	{
		memcpy(m_dataConcatenatePos, data, length);
		m_dataConcatenatePos += length;
	}
}

bool Png::Decompress()
{
	// Use zlib to decompress image data
	return uncompress(m_pixels, &m_imageSize, m_dataStartPos, m_dataConcatenatePos - m_dataStartPos) == Z_OK;
}

void Png::ApplyFiltering()
{
	unsigned char *currentRow = m_pixels;
	unsigned char *lastRow = m_pixels + 1;
	unsigned char *compactedPosition = m_pixels;

	// Apply filter to each scanline - each scanline can have a different filter
	for (unsigned i = 0; i < m_height; i++)
	{
		switch (currentRow[0])
		{
			case NO_FILTER:
				break;
			case SUB:
				FilterSub(currentRow + 1);
				break;
			case UP:
				FilterUp(currentRow + 1, lastRow);
				break;
			case AVERAGE:
				FilterAverage(currentRow + 1, lastRow);
				break;
			case PAETH:
				FilterPaeth(currentRow + 1, lastRow);
				break;
		}

		lastRow = compactedPosition;

		// Compact data to remove filter type bytes
		// fRGBRGBfRGBRGB becomes 
		// RGBRGBbfRGBRGB then
		// RGBRGBRGBRGBgb 
		memcpy(compactedPosition, currentRow + 1, m_width * m_bpp/8);
		compactedPosition += m_width * m_bpp/8;
		
		currentRow += m_width * m_bpp/8 + 1;

		m_imageSize = m_width * m_height * (m_bpp/8);
	}
}

void Png::FilterSub(unsigned char *scanline)
{
	// Store last pixel's data, init to 0
	unsigned char *last = new unsigned char[m_bpp/8];
	for (unsigned i = 0; i < m_bpp/8; i++)
		last[i] = 0;

	// Add pixel to the left to current pixel
	for (unsigned i = 0; i < m_width * m_bpp/8; i++)
	{
		int lastIndex = i % (m_bpp/8);
		scanline[i] = (scanline[i] + last[lastIndex]) % 256;
		last[lastIndex] = scanline[i];
	}

	delete[] last;
}

void Png::FilterUp(unsigned char *scanline, unsigned char *previous)
{
	if (scanline != previous)
	{
		// Add pixel above to current pixel
		for (unsigned i = 0; i < m_width * m_bpp/8; i++)
		{
			scanline[i] = (scanline[i] + previous[i]) % 256;
		}
	}
}

void Png::FilterAverage(unsigned char *scanline, unsigned char *previous)
{
	// Store last pixel's data, init to 0
	unsigned char *last = new unsigned char[m_bpp/8];
	for (unsigned i = 0; i < m_bpp/8; i++)
		last[i] = 0;

	// Add the average of the pixel to the left and the pixel above to the current pixel
	if (scanline != previous)
	{
		for (unsigned i = 0; i < m_width * m_bpp/8; i++)
		{
			scanline[i] = (scanline[i] + (last[i % (m_bpp/8)] + previous[i]) / 2) % 256;
			last[i % (m_bpp/8)] = scanline[i];
		}
	}
	else // If first scanline, assume previous row was all 0s
	{
		for (unsigned i = 0; i < m_width * m_bpp/8; i++)
		{
			scanline[i] = (scanline[i] + last[i % (m_bpp/8)] / 2) % 256;
			last[i % (m_bpp/8)] = scanline[i];
		}
	}

	delete[] last;
}

void Png::FilterPaeth(unsigned char *scanline, unsigned char *previous)
{
	int a, b, c;
	int p, pa, pb, pc;

	// Store last pixel's data, init to 0
	unsigned char *last = new unsigned char[m_bpp/8];
	for (unsigned i = 0; i < m_bpp/8; i++)
		last[i] = 0;

	// Store last pixel of the previous scanline's data, init to 0
	unsigned char *prevLast = new unsigned char[m_bpp/8];
	for (unsigned i = 0; i < m_bpp/8; i++)
		prevLast[i] = 0;

	for (unsigned i = 0; i < m_width * m_bpp/8; i++)
	{
		// get index of current pixel component (r,g,b,a...)
		int lastIndex = i % (m_bpp/8);

		a = last[lastIndex]; // a is pixel to the left

		if (scanline != previous)
			b = previous[i]; // b is pixel above
		else
			b = 0;

		c = prevLast[lastIndex]; // c is pixel above and to the left

		// Paeth calculates each pixel by trying to predict which of the adjacent pixels
		// will be most similar to the current one
		p = a + b - c;
		pa = abs(p - a);
		pb = abs(p - b);
		pc = abs(p - c);

		if (pa <= pb && pa <= pc)
			scanline[i] = (scanline[i] + a) % 256;
		else if (pb <= pc)
			scanline[i] = (scanline[i] + b) % 256;
		else
			scanline[i] = (scanline[i] + c) % 256;

		// Update last pixel buffers
		last[lastIndex] = scanline[i];
		prevLast[lastIndex] = b;
	}

	delete[] last;
	delete[] prevLast;
}

void Png::ApplyPalette()
{
	if (!m_readPLTE)
		return;

	// Allocate memory for palette indexes
	m_pixelIndexes = new unsigned char[m_width * m_height + m_height];
	memcpy(m_pixelIndexes, m_pixels, m_width * m_height + m_height);

	unsigned char *pixelPosition = m_pixels;

	for (unsigned i = 0; i < m_width * m_height + m_height; i++)
	{
		if (i % (m_width + 1) == 0)
		{
			// Copy filter type byte
			memcpy(pixelPosition, &m_pixelIndexes[i], 1);
			pixelPosition++;
		}
		else
		{
			// Copy RBG bytes from palette
			memcpy(pixelPosition, m_palette + m_pixelIndexes[i] * 3, 3);
			pixelPosition += 3;
		}
	}

	// Free pixel index data
	delete[] m_pixelIndexes;
}

void Png::FlipRowOrder() 
{
	unsigned char *tempBuffer = new unsigned char[m_imageSize];
	int increment = m_width * (m_bpp/8);
	unsigned char *buffer = tempBuffer + m_imageSize - increment;
	unsigned char *data = m_pixels;

	for (unsigned i = 0; i < m_height; i++)
	{
		memcpy(buffer, data, increment);
		buffer -= increment;
		data += increment;
	}

	delete m_pixels;
	m_pixels = tempBuffer;
}

unsigned Png::GetWidth() const
{
	return m_width;
}

unsigned Png::GetHeight() const
{
	return m_height;
}

unsigned Png::GetBPP() const
{
	return m_bpp;
}

unsigned long Png::GetSize() const
{
	return m_imageSize;
}

unsigned char* Png::GetPixels() const
{
	return m_pixels;
}

Colour Png::GetPixel(unsigned x, unsigned y) const
{
	if (m_bpp == 32)
	{
		float r, g, b, a;
		int position = (y * m_width + x) * (m_bpp/8);
		r = m_pixels[position] / 255.0f;
		g = m_pixels[position + 1] / 255.0f;
		b = m_pixels[position + 2] / 255.0f;
		a = m_pixels[position + 3] / 255.0f;
		return Colour(r, g, b, a);
	}
	else if (m_bpp == 24)
	{
		float r, g, b;
		int position = (y * m_width + x) * (m_bpp/8);
		r = m_pixels[position] / 255.0f;
		g = m_pixels[position + 1] / 255.0f;
		b = m_pixels[position + 2] / 255.0f;
		return Colour(r, g, b);
	}
	else
	{
		float l;
		int position = (y * m_width + x) * (m_bpp/8);
		l = m_pixels[position] / 255.0f;
		return Colour(l);
	}
}
