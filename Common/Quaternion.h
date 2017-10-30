
#ifndef QUATERNION_H
#define	QUATERNION_H

#include "Vector3.h"

/*
 * @file	Quaternion.h/.cpp
 * @brief	Represents 3D rotation.
 * @author	Matt Drage
 * @date	13/12/2012
 */
class Quaternion
{
	public:
		float w, x, y, z;

		// Constructors
		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(const Vector3 &axis, float angle);
		Quaternion(const Vector3 &euler);
		Quaternion(const Quaternion &other);

		// Identity
		void SetIdentity();
		static Quaternion Identity();

		// Operators
		Quaternion operator* (const Quaternion &other) const;
		void operator*= (const Quaternion &other);
		Vector3 operator* (const Vector3 &vector) const;
		Quaternion operator+ (const Quaternion &other) const;
		Quaternion operator+= (const Quaternion &other);
		Quaternion operator- (const Quaternion &other) const;
		Quaternion operator-= (const Quaternion &other);
		Quaternion operator* (float scale) const;
		Quaternion operator*= (float scale);
		Quaternion operator/ (float scale) const;
		Quaternion operator/= (float scale);
		Quaternion operator- () const;
		void operator= (const Quaternion &other);

		// Magnitude
		float Magnitude() const;
		float SqrMagnitude() const;
	
		// Normalisation
		void Normalize();
		Quaternion GetNormalized() const;
	
		// Inverse
		Quaternion GetConjugate() const;
		void Invert();
		Quaternion GetInverse() const;

		// Dot product
		float Dot(const Quaternion &other) const;

		// Create quaternion
		void LookAt(const Vector3 &origin, const Vector3 &point);
		void FromAxisAngle(const Vector3 &axis, float angle);
		void FromEuler(float pitch, float yaw, float roll);
		void FromEuler(const Vector3 &rotation);

		// Convert
		void ToAxisAngle(Vector3 *axis, float *angle) const;
		Vector3 ToEuler() const;

		// Interpolation
		static Quaternion Slerp(const Quaternion &from, const Quaternion &to, float percent);
		static Quaternion Lerp(const Quaternion &from, const Quaternion &to, float percent);
};

#endif
