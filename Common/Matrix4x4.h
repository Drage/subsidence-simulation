
/*
 * @file	Matrix4x4.h/.cpp
 * @brief	4x4 transformation Matrix.
 * @details	Uses column-major indexing (OpenGL native).
 * @author	Matt Drage
 * @date	13/12/2012
 */

#ifndef MATRIX4X4_H
#define	MATRIX4X4_H

#include "Vector3.h"
#include "Quaternion.h"

class Matrix4x4
{
	public:
		// Constructor - set to identity
		Matrix4x4();
		void SetIdentity();
		static Matrix4x4 Identity();

		// Operators
		Matrix4x4 operator* (const Matrix4x4 &other) const;
		Matrix4x4 operator+ (const Matrix4x4 &other) const;
		Matrix4x4 operator- (const Matrix4x4 &other) const;
		Vector3 operator* (const Vector3 &point) const;
		void operator*= (const Matrix4x4 &other);
		void operator+= (const Matrix4x4 &other);
		void operator-= (const Matrix4x4 &other);
	
		// Create matrix transform
		void SetTranslation(const Vector3 &translation);
		void SetScale(const Vector3 &scale);
		void SetRotation(const Vector3 &axis, float angle);
		void SetRotation(const Vector3 &rotation);
		void SetRotation(const Vector3 &right, const Vector3 &up, const Vector3 &forward);
		void SetRotation(const Quaternion &quaternion);

		// Transposition
		void Transpose();
		Matrix4x4 GetTranspose() const;

		// Camera & projection vertex transforms
		void SetProjectionPerspective(float fov, float apectRatio, float nearClip, float farClip);
		void SetProjectionOrthographic(int width, int height);
		void SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &up = Vector3(0, 1, 0));

		// Decomposition
		Vector3 GetTranslation() const;
		Vector3 GetScale() const;
		Quaternion GetRotation() const;			

		// Array indexing
		float& operator[] (int i);
		const float& operator[] (int i) const;
		const float* ToArray() const;

	private:
		float m_values[16];
};

#endif
