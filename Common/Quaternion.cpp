
#include "Quaternion.h"
#include <cmath>
#include "MathUtils.h"
#include "Matrix4x4.h"

Quaternion::Quaternion()
{
	SetIdentity();
}

Quaternion::Quaternion(float x, float y,  float z, float w)
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quaternion::Quaternion(const Vector3 &axis, float angle)
{
	FromAxisAngle(axis, angle);
}

Quaternion::Quaternion(const Vector3 &euler)
{
	FromEuler(euler);
}

Quaternion::Quaternion(const Quaternion &other)
{
	w = other.w;
	x = other.x;
	y = other.y;
	z = other.z;
}

void Quaternion::SetIdentity()
{
	w = 1;
	x = 0;
	y = 0;
	z = 0;
}

Quaternion Quaternion::Identity()
{
	return Quaternion(0, 0, 0, 1);
}

Quaternion Quaternion::operator* (const Quaternion &other) const
{
	return Quaternion(w * other.x + x * other.w + y * other.z - z * other.y,
	                  w * other.y + y * other.w + z * other.x - x * other.z,
	                  w * other.z + z * other.w + x * other.y - y * other.x,
	                  w * other.w - x * other.x - y * other.y - z * other.z);
}

void Quaternion::operator*= (const Quaternion &other)
{
	*this = *this * other;
}

Vector3 Quaternion::operator* (const Vector3 &vec) const
{
	Vector3 vn = vec.GetNormalized();
 
	Quaternion vecQuat, resQuat;
	vecQuat.x = vn.x;
	vecQuat.y = vn.y;
	vecQuat.z = vn.z;
	vecQuat.w = 0.0f;
 
	resQuat = vecQuat * GetConjugate();
	resQuat = *this * resQuat;
 
	return (Vector3(resQuat.x, resQuat.y, resQuat.z));
}

Quaternion Quaternion::operator+ (const Quaternion &other) const
{
	return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quaternion Quaternion::operator+= (const Quaternion &other)
{
	x += other.x;
	y += other.y; 
	z += other.z; 
	w += other.w;
	return *this;
}

Quaternion Quaternion::operator- (const Quaternion &other) const
{
	return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quaternion Quaternion::operator-= (const Quaternion &other)
{
	x -= other.x;
	y -= other.y; 
	z -= other.z; 
	w -= other.w;
	return *this;
}

Quaternion Quaternion::operator* (float scale) const
{
	return Quaternion(x * scale, y * scale, z * scale, w * scale);
}

Quaternion Quaternion::operator*= (float scale)
{
	x *= scale;
	y *= scale;
	z *= scale; 
	w *= scale;
	return *this;
}

Quaternion Quaternion::operator/ (float scale) const
{
	return Quaternion(x / scale, y / scale, z / scale, w / scale);
}

Quaternion Quaternion::operator/= (float scale)
{
	x /= scale;
	y /= scale;
	z /= scale; 
	w /= scale;
	return *this;
}

Quaternion Quaternion::operator- () const
{
	return Quaternion(-x, -y, -z, -w);
}

void Quaternion::operator= (const Quaternion &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
}

float Quaternion::Magnitude() const
{
	return sqrt(w * w + x * x + y * y + z * z);
}

float Quaternion::SqrMagnitude() const
{
	return w * w + x * x + y * y + z * z;
}

void Quaternion::Normalize()
{
	float mag2 = SqrMagnitude();

    if (std::abs(mag2) > EPSILON && std::abs(mag2 - 1.0f) > EPSILON)
	{
		float mag = Magnitude();
		*this /= mag;
	}
}

Quaternion Quaternion::GetNormalized() const
{
	float mag2 = SqrMagnitude();
	Quaternion normalized(*this);

    if (std::abs(mag2) > EPSILON && std::abs(mag2 - 1.0f) > EPSILON)
	{
		float mag = Magnitude();
		normalized /= mag;
	}

	return normalized;
}

Quaternion Quaternion::GetConjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

void Quaternion::Invert()
{
	*this = GetConjugate() / SqrMagnitude();
}

Quaternion Quaternion::GetInverse() const
{
	return GetConjugate() / SqrMagnitude();
}

float Quaternion::Dot(const Quaternion &other) const
{
	return w * other.w + x * other.x + y * other.y + z * other.z;
}

void Quaternion::LookAt(const Vector3 &origin, const Vector3 &point)
{
	Vector3 direction = (point - origin);
	Vector3 right = Vector3(0, 1, 0).Cross(direction).GetNormalized();
	Vector3 backwards = right.Cross(Vector3(0, 1, 0)).GetNormalized();
	Vector3 up = backwards.Cross(right);
	Matrix4x4 rotMatrix;
	rotMatrix.SetRotation(right, up, backwards);
	(*this) = rotMatrix.GetRotation();
}

void Quaternion::FromAxisAngle(const Vector3 &axis, float angle)
{
	angle *= 0.5f;
	Vector3 vn = axis.GetNormalized();
 
	float sinAngle = sin(angle);
 
	x = (vn.x * sinAngle);
	y = (vn.y * sinAngle);
	z = (vn.z * sinAngle);
	w = cos(angle);
}

void Quaternion::FromEuler(float x, float y, float z)
{
	float rx = DegToRad(x) / 2;
	float ry = DegToRad(y) / 2;
	float rz = DegToRad(z) / 2;
 
	float sinX = sin(rx);
	float sinY = sin(ry);
	float sinZ = sin(rz);
	float cosX = cos(rx);
	float cosY = cos(ry);
	float cosZ = cos(rz);
 
	this->w = cosZ * cosY * cosX + sinZ * sinY * sinX;
	this->x = cosZ * cosY * sinX - sinZ * sinY * cosX;
	this->y = cosZ * sinY * cosX + sinZ * cosY * sinX;
	this->z = sinZ * cosY * cosX - cosZ * sinY * sinX;
 
	Normalize();
}

void Quaternion::FromEuler(const Vector3 &rotation)
{
	FromEuler(rotation.x, rotation.y, rotation.z);
}

void Quaternion::ToAxisAngle(Vector3 *axis, float *angle) const
{
	*angle = acos(w);
	float invSinAngle = 1.0f / sin(*angle);
	axis->x = x * invSinAngle;
	axis->y = y * invSinAngle;
	axis->z = z * invSinAngle;
	*angle = RadToDeg((*angle) * 2);
}

Vector3 Quaternion::ToEuler() const
{
	Vector3 euler;

	float test = x*y + z*w;

	if (test > 0.5f - EPSILON)
	{
		euler.y = 2 * atan2(x,w);
		euler.x = PI/2;
		euler.z = 0;
	}
	else if (test < -0.5f + EPSILON)
	{
		euler.y = -2 * atan2(x,w);
		euler.x = -PI/2;
		euler.z = 0;
	}
	else
	{
		euler.y = atan2(2*y*w-2*x*z, 1 - 2*y*y - 2*z*z);
		euler.x = asin(2*x*y + 2*z*w);
		euler.z = atan2(2*x*w-2*y*z, 1 - 2*x*x - 2*z*z);
	}

	euler.x = RadToDeg(euler.x);
	euler.y = RadToDeg(euler.y);
	euler.z = RadToDeg(euler.z);

	return euler;
}

Quaternion Quaternion::Slerp(const Quaternion &from, const Quaternion &to, float percent)
{
	Quaternion temp;
	float dot = from.Dot(to);

	//if dot < 0 angle is greater than 90 degrees so invert
	if (dot < 0)
	{
		dot = -dot;
		temp = -to;
	} 
	else 
		temp = to;
		
	if (dot < 0.95f)
	{
		float angle = acosf(dot);
		return (from * sin(angle * (1 - percent)) + temp * sin(angle * percent)) / sin(angle);
	} 
	else // use Lerp for small angle
		return Lerp(from, temp, percent);		
}

Quaternion Quaternion::Lerp(const Quaternion &from, const Quaternion &to, float percent)
{
	return (from * (1 - percent) + to * percent).GetNormalized(); 
}
