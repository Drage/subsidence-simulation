
#include "Vector2.h"
#include "MathUtils.h"
#include <sstream>
#include <stdexcept>
#include <cmath>

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2(float xy)
{
	x = xy;
	y = xy;
}

Vector2::Vector2(const Vector2 &other)
{
	x = other.x;
	y = other.y;
}

Vector2 Vector2::operator+ (const Vector2 &other) const
{
   return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator- (const Vector2 &other) const
{
   return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator- () const
{
   return Vector2(-x, -y);
}

Vector2 Vector2::operator* (float operand) const
{
   return Vector2(x * operand, y * operand);
}

Vector2 Vector2::operator/ (float operand) const
{
   return Vector2(x / operand, y / operand);
}

void Vector2::operator+= (const Vector2 &other)
{
	x += other.x;
	y += other.y;
}

void Vector2::operator-= (const Vector2 &other)
{
	x -= other.x;
	y -= other.y;
}

void Vector2::operator*= (float operand)
{
	x *= operand;
	y *= operand;
}

void Vector2::operator/= (float operand)
{
	x /= operand;
	y /= operand;
}

void Vector2::operator= (const Vector2 &other)
{
	x = other.x;
	y = other.y;
}

bool Vector2::operator== (const Vector2 &other)
{
    return (std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON);
}

bool Vector2::operator!= (const Vector2 &other)
{
	return (std::abs(x - other.x) > EPSILON || std::abs(y - other.y) > EPSILON);
}

float Vector2::Magnitude() const
{
	return sqrt((x * x) + (y * y));
}

float Vector2::MagnitudeSqr() const
{
	return (x * x) + (y * y);
}

Vector2 Vector2::Normalize()
{
	float magnitude = Magnitude();
	if (magnitude)
		(*this) /= magnitude;
	return *this;
}

Vector2 Vector2::GetNormalized() const
{
	float magnitude = Magnitude();
	Vector2 normalized(*this);

	if (magnitude)
		normalized /= magnitude;

	return normalized;
}

Vector2 Vector2::Scale(float newScale)
{
	float scaleFactor = newScale / Magnitude();
	if (scaleFactor)
		(*this) *= scaleFactor;
	return *this;
}

float Vector2::Dot(const Vector2 &other) const
{
	return x * other.x + y * other.y;
}

Vector2 Vector2::Rotate(float angle)
{
	float cosOfAngle = cos(angle);
	float sinOfAngle = sin(angle);

	float rotatedX = x * cosOfAngle - y * sinOfAngle;
	float rotatedY = x * sinOfAngle + y * cosOfAngle;

	x = rotatedX;
	y = rotatedY;

	return *this;
}

float Vector2::Angle(const Vector2 &other) const
{
	Vector2 v1 = this->GetNormalized();
	Vector2 v2 = other.GetNormalized();
	float dot = v1.Dot(v2);
	return acos(dot);
}

Vector2 Vector2::Reflect(const Vector2 &normal) const
{
	float dot = this->Dot(normal);
	return *this + normal * 2 * -dot;
}

Vector2 Vector2::Lerp(const Vector2 &from, const Vector2 &to, float percent)
{
	return from + (to - from) * percent;
}

float& Vector2::operator[] (int i)
{
	if (i < 0 || i > 1)
		throw std::out_of_range("Invalid vector index");
	
	switch (i)
	{
		case 0: return x;
		case 1: return y;
		default: return x;
	}
}

const float& Vector2::operator[] (int i) const
{
	if (i < 0 || i > 1)
		throw std::out_of_range("Invalid vector index");
	
	switch (i)
	{
		case 0: return x;
		case 1: return y;
		default: return x;
	}
}

float* Vector2::ToArray()
{
	static float vec2Array[2];
	vec2Array[0] = x;
	vec2Array[1] = y;
	return vec2Array;
}

std::istream& operator>> (std::istream &stream, Vector2 &vector)
{
	stream >> vector.x >> vector.y;
	return stream;
}

std::ostream& operator<< (std::ostream &stream, const Vector2 &vector)
{
	stream << vector.x << " " << vector.y;
	return stream;
}
