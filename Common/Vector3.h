
/*
 * @file	Vector3.h/.cpp
 * @brief	Three-dimensional vector.
 * @author	Matt Drage
 * @date	14/12/2012
 */

#ifndef VECTOR3_H
#define VECTOR3_H

#include <string>
#include "Vector2.h"

class Vector3
{
	// Stream operators
	friend std::istream& operator>>(std::istream &stream, Vector3 &vector);
	friend std::ostream& operator<<(std::ostream &stream, const Vector3 &vector);

	public:
		float x;
		float y;
		float z;

		// Constructors
		Vector3();
		Vector3(float x, float y, float z);
		Vector3(float xyz);
		Vector3(const Vector3 &other);
		Vector3(const Vector2 &other);
	
		// Predefined directions
		static Vector3 Up();
		static Vector3 Down();
		static Vector3 Left();
		static Vector3 Right();
		static Vector3 Forward();
		static Vector3 Backward();

		// Operators
		Vector3 operator+ (const Vector3 &other) const;
		Vector3 operator- (const Vector3 &other) const;
		Vector3 operator- () const;
		Vector3 operator* (float operand) const;
		Vector3 operator* (const Vector3 &other) const;
		Vector3 operator/ (float operand) const;
		Vector3 operator/ (const Vector3 &other) const;

		// Assignment operators
		void operator+= (const Vector3 &other);
		void operator-= (const Vector3 &other);
		void operator*= (float operand);
		void operator/= (float operand);
		void operator= (const Vector3 &other);
	
		// Comparison operators
		bool operator== (const Vector3 &other);
		bool operator!= (const Vector3 &other);
		bool operator< (const Vector3 &other) const;
		bool operator> (const Vector3 &other) const;

		// Magnitude
		float Magnitude() const;
		float MagnitudeSqr() const;

		// Normalisation
		Vector3 Normalize();
		Vector3 GetNormalized() const;
		Vector3 Scale(float scale);

		// Geometeric functions
		float Dot(const Vector3 &other) const;
		Vector3 Cross(const Vector3 &other) const;
	
		// Angles
		float Angle(const Vector3 &other) const;
		Vector3 Reflect(const Vector3 &normal) const;

		// Interpolation
		static Vector3 Lerp(const Vector3 &from, const Vector3 &to, float percent);

		// Array access
		float& operator[] (int i);
		const float& operator[] (int i) const;
		float* ToArray();
};

#endif
