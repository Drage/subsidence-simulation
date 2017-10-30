
/*
 * @file	Colour.h/.cpp
 * @brief	Stores RGBA colour information.
 * @author	Matt Drage
 * @date	22/01/2012
 */

#ifndef COLOUR_H
#define COLOUR_H

#include "Vector3.h"

class Colour
{
	// Stream operators
	friend std::istream& operator>>(std::istream &stream, Colour &colour);
	friend std::ostream& operator<<(std::ostream &stream, const Colour &colour);

	public:
		float r;
		float g;
		float b;
		float a;
	
		// Default colours
		static Colour White();
		static Colour Black();
		static Colour Cyan();
		static Colour Magenta();
		static Colour Yellow();
		static Colour Red();
		static Colour Blue();
		static Colour Green();
		static Colour LightBlue();

		// Constructors
		Colour();
		Colour(float r, float g, float b, float a = 1.0f);
		Colour(float luminance);
		Colour(const Vector3 &vec3);

		// Additive blending
		Colour operator+ (const Colour &other) const;
		void operator+= (const Colour &other);

		// Assignment operators
		void operator= (const Colour &other);

		// Comparison operators
		bool operator== (const Colour &other) const;
		bool operator!= (const Colour &other) const;
		bool operator< (const Colour &other) const;

		// Modulate intensity
		Colour operator* (float scalar) const;

		// Array access
		float& operator[] (int i);
		const float& operator[] (int i) const;
	
		// Conversions
		float* ToArray() const;
		Vector3 ToVector3() const;
		float ToGreyscale() const;
		static Colour FromHSV(float h, float s, float v);
};

#endif
