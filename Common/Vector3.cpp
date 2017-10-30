
#include "Vector3.h"
#include "MathUtils.h"
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(float xyz)
{
	x = xyz;
	y = xyz;
	z = xyz;
}

Vector3::Vector3(const Vector3 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

Vector3::Vector3(const Vector2 &other)
{
	x = other.x;
	y = other.y;
	z = 0.0f;
}

Vector3 Vector3::Up()
{
	return Vector3(0, 1, 0);
}

Vector3 Vector3::Down()
{
	return Vector3(0, -1, 0);
}

Vector3 Vector3::Left()
{
	return Vector3(-1, 0, 0);
}

Vector3 Vector3::Right()
{
	return Vector3(1, 0, 0);
}

Vector3 Vector3::Forward()
{
	return Vector3(0, 0, 1);
}

Vector3 Vector3::Backward()
{
	return Vector3(0, 0, -1);
}

Vector3 Vector3::operator+ (const Vector3 &other) const
{
   return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator- (const Vector3 &other) const
{
   return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator- () const
{
   return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator* (float operand) const
{
   return Vector3(x * operand, y * operand, z * operand);
}

Vector3 Vector3::operator* (const Vector3 &other) const
{
	return Vector3(x * other.x, y * other.y, z * other.z);
}

Vector3 Vector3::operator/ (float operand) const
{
   return Vector3(x / operand, y / operand, z / operand);
}

Vector3 Vector3::operator/ (const Vector3 &other) const
{
	return Vector3(x / other.x, y / other.y, z / other.z);
}

void Vector3::operator+= (const Vector3 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

void Vector3::operator-= (const Vector3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

void Vector3::operator*= (float operand)
{
	x *= operand;
	y *= operand;
	z *= operand;
}

void Vector3::operator/= (float operand)
{
	x /= operand;
	y /= operand;
	z /= operand;
}

void Vector3::operator= (const Vector3 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

bool Vector3::operator== (const Vector3 &other)
{
    return (std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON && std::abs(z - other.z) < EPSILON);
}

bool Vector3::operator!= (const Vector3 &other)
{
	return (std::abs(x - other.x) > EPSILON || std::abs(y - other.y) > EPSILON || std::abs(z - other.z) > EPSILON);
}

bool Vector3::operator< (const Vector3 &other) const
{
	return Magnitude() < other.Magnitude();
}

bool Vector3::operator> (const Vector3 &other) const
{
	return Magnitude() > other.Magnitude();
}

float Vector3::Magnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

float Vector3::MagnitudeSqr() const
{
	return (x * x) + (y * y) + (z * z);
}

Vector3 Vector3::Normalize()
{
	float magnitude = Magnitude();

	if (magnitude)
		(*this) /= magnitude;

	return *this;
}

Vector3 Vector3::GetNormalized() const
{
	float magnitude = Magnitude();
	Vector3 normalized(*this);

	if (magnitude)
		normalized /= magnitude;

	return normalized;
}

Vector3 Vector3::Scale(float newScale)
{
	float scaleFactor = newScale / Magnitude();
	
	if (scaleFactor)
		(*this) *= scaleFactor;
	
	return *this;
}

float Vector3::Dot(const Vector3 &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(const Vector3 &other) const
{
	return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

float Vector3::Angle(const Vector3 &other) const
{
	Vector3 v1 = this->GetNormalized();
	Vector3 v2 = other.GetNormalized();
	float dot = v1.Dot(v2);
	return acos(dot);
}

Vector3 Vector3::Reflect(const Vector3 &normal) const
{
	float dot = this->Dot(normal);
	return *this + normal * 2 * -dot;
}

Vector3 Vector3::Lerp(const Vector3 &from, const Vector3 &to, float percent)
{
	return from + (to - from) * percent;
}

float& Vector3::operator[] (int i)
{
	if (i < 0 || i > 3)
		throw std::out_of_range("Invalid vector index");
	
	switch (i)
	{
		case 0:	return x;
		case 1: return y;
		case 2:	return z;
		default: return x;
	}
}

const float& Vector3::operator[] (int i) const
{
	if (i < 0 || i > 3)
		throw std::out_of_range("Invalid vector index");
	
	switch (i)
	{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return x;
	}
}

float* Vector3::ToArray()
{
	static float vec3Array[3];
	vec3Array[0] = x;
	vec3Array[1] = y;
	vec3Array[2] = z;
	return vec3Array;
}

std::istream& operator>> (std::istream &stream, Vector3 &vector)
{
	stream >> vector.x >> vector.y >> vector.z;
	return stream;
}

std::ostream& operator<< (std::ostream &stream, const Vector3 &vector)
{
	stream << vector.x << " " << vector.y << " " << vector.z;
	return stream;
}
