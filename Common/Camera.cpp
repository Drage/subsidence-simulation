
#include "Camera.h"
#include "Graphics.h"

Camera::Camera()
{
	m_position = Vector3(0, 0, -10);
	m_focus = Vector3(0, 0, 0);
	m_up = Vector3(0, 1, 0);
	m_fov = 60;
	m_nearClip = 0.1f;
	m_farClip = 1000.0f;
}

Camera::Camera(const Vector3 &position, const Vector3 &focus, const Vector3 &up, float fov, float nearClip, float farClip)
{
	m_position = position;
	m_focus = focus;
	m_up = up;
	m_fov = fov;
	m_nearClip = nearClip;
	m_farClip = farClip;
}

void Camera::SetPosition(const Vector3 &position)
{
	m_position = position;
}

void Camera::SetFocus(const Vector3 &focus)
{
	m_focus = focus;
}

void Camera::SetUpVector(const Vector3 &up) 
{
	m_up = up;
}

void Camera::SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &up)
{
	m_position = position;
	m_focus = focus;
	m_up = up;
}

void Camera::SetFOV(float fov)
{
	m_fov = fov;
}

void Camera::SetNearClip(float nearClip)
{
	m_nearClip = nearClip;
}

void Camera::SetFarClip(float farClip)
{
	m_farClip = farClip;
}

void Camera::SetProjectionPerspective(float fov, float nearClip, float farClip)
{
	m_fov = fov;
	m_nearClip = nearClip;
	m_farClip = farClip;
}

Vector3 Camera::GetPosition() const
{
	return m_position;
}

Vector3 Camera::GetFocus() const
{
	return m_focus;
}

Vector3 Camera::GetUpVector() const
{
	return m_up;
}

float Camera::GetFOV() const
{
	return m_fov;
}

float Camera::GetNearClip() const
{
	return m_nearClip;
}

float Camera::GetFarClip() const
{
	return m_farClip;
}

const Matrix4x4& Camera::GetViewMatrix()
{
	m_viewMatrix.SetView(m_position, m_focus, m_up);
	return m_viewMatrix;
}

const Matrix4x4& Camera::GetProjectionMatrix()
{
	float aspectRatio = 1.0f * GetWindowWidth() / GetWindowHeight();
	m_projectionMatrix.SetProjectionPerspective(m_fov, aspectRatio, m_nearClip, m_farClip);
	return m_projectionMatrix;
}
