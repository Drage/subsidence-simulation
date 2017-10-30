
#include "Colour.h"
#include "MathUtils.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <stdio.h>
#include <cmath>

Colour Colour::White()
{
	return Colour(1, 1, 1);
}

Colour Colour::Black()
{
	return Colour(0, 0, 0);
}

Colour Colour::Cyan()
{
	return Colour(0, 1, 1);
}

Colour Colour::Magenta()
{
	return Colour(1, 0, 1);
}

Colour Colour::Yellow()
{
	return Colour(1, 1, 0);
}

Colour Colour::Red()
{
	return Colour(1, 0, 0);
}

Colour Colour::Blue()
{
	return Colour(0, 0, 1);
}

Colour Colour::Green()
{
	return Colour(0, 1, 0);
}

Colour Colour::LightBlue()
{
	return Colour(70, 151, 223);
}

Colour::Colour()
{
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->a = 1;
}

Colour::Colour(float r, float g, float b, float a)
{
	// Rescale input if given as 0-255 range instead of 0-1
	if (r > 1) r /= 255.0f;
	if (g > 1) g /= 255.0f;
	if (b > 1) b /= 255.0f;
	if (a > 1) a /= 255.0f;

	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Colour::Colour(float l)
{
	this->r = l;
	this->g = l;
	this->b = l;
	this->a = 1;
}

Colour::Colour(const Vector3 &vec3)
{
	r = vec3.x;
	g = vec3.y;
	b = vec3.z;
	a = 1;
}

Colour Colour::operator+ (const Colour &other) const
{
	float rx, gx, bx, ax;

	ax = 1 - (1 - a) * (1 - other.a);
	rx = r * a / ax + other.r * other.a * (1 - a) / ax;
	gx = g * a / ax + other.g * other.a * (1 - a) / ax;
	bx = b * a / ax + other.b * other.a * (1 - a) / ax;

	return Colour(rx, gx, bx, ax);
}

void Colour::operator+= (const Colour &other)
{
	*this = (*this) + other;
}

void Colour::operator= (const Colour &other)
{
	r = other.r;
	g = other.g;
	b = other.b;
	a = other.a;
}

bool Colour::operator== (const Colour &other) const
{
    return (std::abs(r - other.r) < EPSILON && std::abs(g - other.g) < EPSILON && std::abs(b - other.b) < EPSILON && std::abs(a - other.a) < EPSILON);
}

bool Colour::operator!= (const Colour &other) const
{
	return (std::abs(r - other.r) > EPSILON || std::abs(g - other.g) > EPSILON || std::abs(b - other.b) > EPSILON || std::abs(a - other.a) > EPSILON);
}

bool Colour::operator< (const Colour &other) const
{
	if (r < other.r)
		return true;
	else if (r > other.r)
		return false;
	else
	{
		if (g < other.g)
			return true;
		else if (g > other.g)
			return false;
		else
		{
			if (b < other.b)
				return true;
			else if (b > other.b)
				return false;
			else
			{
				if (a < other.a)
					return true;
				else
					return false;
			}
		}
	}
}

Colour Colour::operator* (float scalar) const
{
	return Colour(Clamp(r * scalar, 0.0f, 1.0f), Clamp(g * scalar, 0.0f, 1.0f), 
		Clamp(b * scalar, 0.0f, 1.0f), Clamp(a * scalar, 0.0f, 1.0f));
}

float& Colour::operator[] (int i)
{
	if (i < 0 || i > 3)
		throw std::out_of_range("Invalid colour index");
	
	switch (i)
	{
		case 0: return r;
		case 1: return g;
		case 2: return b;
		case 3: return a;
		default: return r;
	}
}

const float& Colour::operator[] (int i) const
{
	if (i < 0 || i > 3)
		throw std::out_of_range("Invalid colour index");
	
	switch (i)
	{
		case 0: return r;
		case 1: return g;
		case 2: return b;
		case 3: return a;
		default: return r;
	}
}

float* Colour::ToArray() const
{
	static float colourArray[4];
	colourArray[0] = r;
	colourArray[1] = g;
	colourArray[2] = b;
	colourArray[3] = a;
	return colourArray;
}

Vector3 Colour::ToVector3() const
{
	return Vector3(r, g, b);
}

float Colour::ToGreyscale() const
{
	return (r + g + b) / 3;
}

Colour Colour::FromHSV(float h, float s, float v)
{
	float hh, p, q, t, ff;
    long i;
	float r, g, b;
	
    if (s <= 0.0f) 
		return Colour();
	
    hh = h;
    if (hh >= 360.0f) 
		hh = 0.0f;
    hh /= 60.0f;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * ff));
    t = v * (1.0f - (s * (1.0f - ff)));
	
    switch (i) 
	{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = p;
			b = q;
			break;
    }
	
	return Colour(r, g, b);
}

std::istream& operator>> (std::istream &stream, Colour &colour)
{
	std::string hex;
	stream >> hex;

	std::string redStr = hex.substr(2, 2);
	std::string greenStr = hex.substr(4, 2);
	std::string blueStr = hex.substr(6, 2);

	unsigned int red, green, blue;
	sscanf(redStr.c_str(), "%x", &red);
	sscanf(greenStr.c_str(), "%x", &green);
	sscanf(blueStr.c_str(), "%x", &blue);

	colour.r = red / 255.0f;
	colour.g = green / 255.0f;
	colour.b = blue / 255.0f;

	return stream;
}

std::ostream& operator<< (std::ostream &stream, const Colour &colour)
{
	int red = (int)(colour.r * 255);
	int green = (int)(colour.g * 255);
	int blue = (int)(colour.b * 255);

	std::stringstream ss;
	ss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex;
	ss << red << green << blue;

	stream << "0x" + ss.str();
	return stream;
}
