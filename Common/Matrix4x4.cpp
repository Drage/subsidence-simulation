
#include "Matrix4x4.h"
#include "MathUtils.h"
#include <stdexcept>

Matrix4x4::Matrix4x4()
{
	SetIdentity();
}

void Matrix4x4::SetIdentity()
{
	// Set all to 0
	for (int i = 0; i < 16; i++)
		m_values[i] = 0.0f;

	// fill diagonal with 1s
	m_values[0] = m_values[5] = m_values[10] = m_values[15] = 1.0f;
}

Matrix4x4 Matrix4x4::Identity()
{
	return Matrix4x4();
}

Matrix4x4 Matrix4x4::operator* (const Matrix4x4 &other) const
{
	Matrix4x4 result;

	// First Column
    result[0 ] = m_values[0]*other[0 ] + m_values[4]*other[1 ] + m_values[8 ]*other[2 ] + m_values[12]*other[3 ];
    result[1 ] = m_values[1]*other[0 ] + m_values[5]*other[1 ] + m_values[9 ]*other[2 ] + m_values[13]*other[3 ];
    result[2 ] = m_values[2]*other[0 ] + m_values[6]*other[1 ] + m_values[10]*other[2 ] + m_values[14]*other[3 ];
    result[3 ] = m_values[3]*other[0 ] + m_values[7]*other[1 ] + m_values[11]*other[2 ] + m_values[15]*other[3 ];
 
    // Second Column
    result[4 ] = m_values[0]*other[4 ] + m_values[4]*other[5 ] + m_values[8 ]*other[6 ] + m_values[12]*other[7 ];
    result[5 ] = m_values[1]*other[4 ] + m_values[5]*other[5 ] + m_values[9 ]*other[6 ] + m_values[13]*other[7 ];
    result[6 ] = m_values[2]*other[4 ] + m_values[6]*other[5 ] + m_values[10]*other[6 ] + m_values[14]*other[7 ];
    result[7 ] = m_values[3]*other[4 ] + m_values[7]*other[5 ] + m_values[11]*other[6 ] + m_values[15]*other[7 ];
 
    // Third Column
    result[8 ] = m_values[0]*other[8 ] + m_values[4]*other[9 ] + m_values[8 ]*other[10] + m_values[12]*other[11];
    result[9 ] = m_values[1]*other[8 ] + m_values[5]*other[9 ] + m_values[9 ]*other[10] + m_values[13]*other[11];
    result[10] = m_values[2]*other[8 ] + m_values[6]*other[9 ] + m_values[10]*other[10] + m_values[14]*other[11];
    result[11] = m_values[3]*other[8 ] + m_values[7]*other[9 ] + m_values[11]*other[10] + m_values[15]*other[11];
 
    // Fourth Column
    result[12] = m_values[0]*other[12] + m_values[4]*other[13] + m_values[8 ]*other[14] + m_values[12]*other[15];
    result[13] = m_values[1]*other[12] + m_values[5]*other[13] + m_values[9 ]*other[14] + m_values[13]*other[15];
    result[14] = m_values[2]*other[12] + m_values[6]*other[13] + m_values[10]*other[14] + m_values[14]*other[15];
    result[15] = m_values[3]*other[12] + m_values[7]*other[13] + m_values[11]*other[14] + m_values[15]*other[15];

	return result;
}

Matrix4x4 Matrix4x4::operator+ (const Matrix4x4 &other) const
{
	Matrix4x4 result;

	for (int i = 0; i < 16; i++)
		result[i] = m_values[i] + other[i];

	return result;
}

Matrix4x4 Matrix4x4::operator- (const Matrix4x4 &other) const
{
	Matrix4x4 result;

	for (int i = 0; i < 16; i++)
		result[i] = m_values[i] - other[i];

	return result;
}

Vector3 Matrix4x4::operator* (const Vector3 &point) const
{
	Vector3 result;

	result[0] = m_values[0]*point[0] + m_values[4]*point[1] + m_values[8 ]*point[2] + m_values[12]*point[3];
    result[1] = m_values[1]*point[0] + m_values[5]*point[1] + m_values[9 ]*point[2] + m_values[13]*point[3];
    result[2] = m_values[2]*point[0] + m_values[6]*point[1] + m_values[10]*point[2] + m_values[14]*point[3];
    result[3] = m_values[3]*point[0] + m_values[7]*point[1] + m_values[11]*point[2] + m_values[15]*point[3];

	return result;
}

void Matrix4x4::operator*= (const Matrix4x4 &other)
{
	*this = (*this) * other;
}

void Matrix4x4::operator+= (const Matrix4x4 &other)
{
	*this = (*this) + other;
}

void Matrix4x4::operator-= (const Matrix4x4 &other)
{
	*this = (*this) - other;
}

void Matrix4x4::SetTranslation(const Vector3 &translation)
{
	SetIdentity();
	m_values[12] = translation.x;
	m_values[13] = translation.y;
	m_values[14] = translation.z;
}

void Matrix4x4::SetScale(const Vector3 &scale)
{
	SetIdentity();
	m_values[0] = scale.x;
	m_values[5] = scale.y;
	m_values[10] = scale.z;
}

void Matrix4x4::SetRotation(const Vector3 &axis, float angle)
{
	float radAngle = DegToRad(angle);
	float cosine = cos(radAngle);
	float sine = sin(radAngle);
	float x2 = axis.x*axis.x;
	float y2 = axis.y*axis.y;
	float z2 = axis.z*axis.z;

	m_values[0] = x2 + (y2 + z2) * cosine; 
	m_values[4] = axis.x * axis.y * (1 - cosine) - axis.z * sine;
	m_values[8] = axis.x * axis.z * (1 - cosine) + axis.y * sine;
	m_values[12]= 0.0f;
	   
	m_values[1] = axis.x * axis.y * (1 - cosine) + axis.z * sine;
	m_values[5] = y2 + (x2 + z2) * cosine;
	m_values[9] = axis.y * axis.z * (1 - cosine) - axis.x * sine;
	m_values[13]= 0.0f;
	   
	m_values[2] = axis.x * axis.z * (1 - cosine) - axis.y * sine;
	m_values[6] = axis.y * axis.z * (1 - cosine) + axis.x * sine;
	m_values[10]= z2 + (x2 + y2) * cosine;
	m_values[14]= 0.0f;
	   
	m_values[3] = 0.0f;
	m_values[7] = 0.0f;
	m_values[11]= 0.0f;
	m_values[15]= 1.0f;
}

void Matrix4x4::SetRotation(const Vector3 &rotation)
{
	Matrix4x4 X, Y, Z;
	float x, y, z;

	x = DegToRad(rotation.x);
	y = DegToRad(rotation.y);
	z = DegToRad(rotation.z);

	Y.SetRotation(Vector3(0, 1, 0), y);
	X.SetRotation(Vector3(1, 0, 0), x);
	Z.SetRotation(Vector3(0, 0, 1), z);

	*this = Y * X * Z;
}

void Matrix4x4::SetRotation(const Vector3 &right, const Vector3 &up, const Vector3 &forward)
{
	m_values[0] = right.x;		m_values[1] = right.y;		m_values[2] = right.z;		m_values[3] = 0;
	m_values[4] = up.x;			m_values[5] = up.y;			m_values[6] = up.z;			m_values[7] = 0;
	m_values[8] = forward.x;	m_values[9] = forward.y;	m_values[10] = forward.z;	m_values[11] = 0;
	m_values[12] = 0;			m_values[13] = 0;			m_values[14] = 0;			m_values[15] = 1;
}
void Matrix4x4::SetRotation(const Quaternion &quat)
{
	// First column
	m_values[0] = 1 - 2 * (quat.y * quat.y + quat.z * quat.z);
    m_values[1] = 2 * (quat.x * quat.y + quat.z * quat.w);
    m_values[2] = 2 * (quat.x * quat.z - quat.y * quat.w);
    m_values[3] = 0;
 
    // Second Column
    m_values[4] = 2 * (quat.x * quat.y - quat.z * quat.w);
    m_values[5] = 1 - 2 * (quat.x * quat.x + quat.z * quat.z);
    m_values[6] = 2 * (quat.z * quat.y + quat.x * quat.w);
    m_values[7] = 0;
 
    // Third Column
    m_values[8] = 2 * (quat.x * quat.z + quat.y * quat.w);
    m_values[9] = 2 * (quat.y * quat.z - quat.x * quat.w);
    m_values[10] = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
    m_values[11] = 0;
 
    // Fourth Column
    m_values[12] = 0;
    m_values[13] = 0;
    m_values[14] = 0;
    m_values[15] = 1;
}

void Matrix4x4::Transpose()
{
	*this = this->GetTranspose();
}

Matrix4x4 Matrix4x4::GetTranspose() const
{
	Matrix4x4 transpose;

	transpose[0 ] = m_values[0];	transpose[1 ] = m_values[4];	transpose[2 ] = m_values[8];	transpose[3 ] = m_values[12];
	transpose[4 ] = m_values[1];	transpose[5 ] = m_values[5];	transpose[6 ] = m_values[9];	transpose[7 ] = m_values[13];
	transpose[8 ] = m_values[2];	transpose[9 ] = m_values[6];	transpose[10] = m_values[10];	transpose[7 ] = m_values[14];
	transpose[12] = m_values[3];	transpose[13] = m_values[7];	transpose[14] = m_values[11];	transpose[15] = m_values[15];

	return transpose;
}

void Matrix4x4::SetProjectionPerspective(float fov, float apectRatio, float nearClip, float farClip)
{
	float size = nearClip * tanf(DegToRad(fov) / 2.0f);
    float left = -size, right = size, bottom = -size / apectRatio, top = size / apectRatio;

	float f = 1.0f / tan(fov * (PI / 360.0f));

	m_values[0] = 2 * nearClip / (right - left);
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
 
    m_values[4] = 0;
    m_values[5] = 2 * nearClip / (top - bottom);
    m_values[6] = 0;
    m_values[7] = 0;
 
    m_values[8] = (right + left) / (right - left);
    m_values[9] = (top + bottom) / (top - bottom);
    m_values[10] = -(farClip + nearClip) / (farClip - nearClip);
    m_values[11] = -1;
 
    m_values[12] = 0;
    m_values[13] = 0;
    m_values[14] = -(2 * farClip * nearClip) / (farClip - nearClip);
    m_values[15] = 0;
}

void Matrix4x4::SetProjectionOrthographic(int width, int height)
{
	float left, right, top, bottom, far, near;
	left = 0;
	right = (float)width;
	bottom = 0;
	top = (float)height;
	near = -1;
	far = 1;

    m_values[0] = 2.0f / (right - left);
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
 
    m_values[4] = 0;
    m_values[5] = 2.0f / (top - bottom);
    m_values[6] = 0;
    m_values[7] = 0;
 
    m_values[8] = 0;
    m_values[9] = 0;
    m_values[10] = -2.0f / (far - near);
    m_values[11] = 0;

    m_values[12] = -(right + left) / (right - left);
    m_values[13] = -(top + bottom) / (top - bottom);
    m_values[14] = -(far + near) / (far - near);
    m_values[15] = 1;
}

void Matrix4x4::SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &upVector)
{
	Vector3 forward = (focus - position).GetNormalized();
	Vector3 up = upVector.GetNormalized();
	Vector3 side = forward.Cross(up).GetNormalized();
	up = side.Cross(forward).GetNormalized();

	// First column
	m_values[0 ] = side[0];
	m_values[1 ] = up[0];
	m_values[2 ] = -forward[0];
	m_values[3 ] = 0.0f;

	// Second column
	m_values[4 ] = side[1];
	m_values[5 ] = up[1];
	m_values[6 ] = -forward[1];
	m_values[7 ] = 0.0f;

	// Third column
	m_values[8 ] = side[2];
	m_values[9 ] = up[2];
	m_values[10] = -forward[2];
	m_values[11] =  0.0f;

	// fourth column
	m_values[12] = 0.0f;
	m_values[13] = 0.0f;
	m_values[14] = 0.0f;
	m_values[15] = 1.0f;

	Matrix4x4 translation;
	translation.SetTranslation(-position);
	*this *= translation;
}

Vector3 Matrix4x4::GetTranslation() const
{
	return Vector3(m_values[12], m_values[13], m_values[14]);
}

Vector3 Matrix4x4::GetScale() const
{
	return Vector3(m_values[0], m_values[5], m_values[10]);
}

Quaternion Matrix4x4::GetRotation() const
{
	Quaternion q;

	q.w = sqrt(Max<float>(0, 1 + m_values[0] + m_values[5] + m_values[10])) / 2.0f; 
	q.x = sqrt(Max<float>(0, 1 + m_values[0] - m_values[5] - m_values[10])) / 2.0f; 
	q.y = sqrt(Max<float>(0, 1 - m_values[0] + m_values[5] - m_values[10])) / 2.0f; 
	q.z = sqrt(Max<float>(0, 1 - m_values[0] - m_values[5] + m_values[10])) / 2.0f; 

	q.x *= Sign<float>(q.x * (m_values[6] - m_values[9]));
	q.y *= Sign<float>(q.y * (m_values[8] - m_values[2]));
	q.z *= Sign<float>(q.z * (m_values[1] - m_values[4]));

	q.Normalize();
	return q;
}

float& Matrix4x4::operator[] (int i)
{
	if (i >= 0 && i < 16)
		return m_values[i];
	else
		throw std::out_of_range("Invalid matrix index");
}

const float& Matrix4x4::operator[] (int i) const
{
	if (i >= 0 && i < 16)
		return m_values[i];
	else
		throw std::out_of_range("Invalid matrix index");
}

const float* Matrix4x4::ToArray() const
{
	return m_values;
}
