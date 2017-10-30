
/*
 * @file	Vector2.h/.cpp
 * @brief	Two-dimensional vector.
 * @author	Matt Drage
 * @date	14/12/2012
 */

#ifndef VECTOR2_H
#define VECTOR2_H

#include <string>

class Vector2
{
	// Stream operators
	friend std::istream& operator>>(std::istream &stream, Vector2 &vector);
	friend std::ostream& operator<<(std::ostream &stream, const Vector2 &vector);

	public:
		float x;
		float y;

		// Constructors
		Vector2();
		Vector2(float x, float y);
		Vector2(float xy);
		Vector2(const Vector2 &other);

		// Operators
		Vector2 operator+ (const Vector2 &other) const;
		Vector2 operator- (const Vector2 &other) const;
		Vector2 operator- () const;
		Vector2 operator* (float operand) const;
		Vector2 operator/ (float operand) const;
	
		// Assignment operators
		void operator+= (const Vector2 &other);
		void operator-= (const Vector2 &other);
		void operator*= (float operand);
		void operator/= (float operand);
		void operator= (const Vector2 &other);

		// Comparison operators
		bool operator== (const Vector2 &other);
		bool operator!= (const Vector2 &other);

		// Magnitude
		float Magnitude() const;
		float MagnitudeSqr() const;

		// Normalisation
		Vector2 Normalize();
		Vector2 GetNormalized() const;
		Vector2 Scale(float scale);

		// Angles
		float Dot(const Vector2 &other) const;
		Vector2 Rotate(float angle);
		float Angle(const Vector2 &other) const;
		Vector2 Reflect(const Vector2 &normal) const;

		// Interpolation
		static Vector2 Lerp(const Vector2 &from, const Vector2 &to, float percent);

		// Array access
		float& operator[] (int i);
		const float& operator[] (int i) const;
		float* ToArray();
};


#endif
