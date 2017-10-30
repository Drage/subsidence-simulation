
#include "OrbitCamera.h"
#include "Input.h"
#include "MathUtils.h"

OrbitCamera::OrbitCamera()
{
	m_offset = (m_position - m_focus);
	m_rotateSensitivity = 1.0f;
	m_panSensitivity = 0.7f;
	m_zoomSensitivity = 50.0f;
	m_sensitivity = 1.0f;
}

OrbitCamera::OrbitCamera(const Vector3 &position, const Vector3 &focus, float sensitivity)
{
	m_position = position;
	m_focus = focus;
	m_offset = (m_position - m_focus);
	m_rotateSensitivity = 1.0f;
	m_panSensitivity = 0.7f;
	m_zoomSensitivity = 50.0f;
	m_sensitivity = sensitivity;
}

void OrbitCamera::Update(double deltaTime)
{
	if (Mouse.left)
		Rotate(-Mouse.motion * m_sensitivity * m_rotateSensitivity * deltaTime);
	else if (Mouse.right)
		Zoom(Mouse.motion.y * m_sensitivity * m_zoomSensitivity * deltaTime);
	
	if (Key.right)
		Rotate(Vector2(10, 0) * m_sensitivity * m_rotateSensitivity * deltaTime);
	if (Key.left)
		Rotate(Vector2(-10, 0) * m_sensitivity * m_rotateSensitivity * deltaTime);
	if (Key.up)
		Rotate(Vector2(0, 10) * m_sensitivity * m_rotateSensitivity * deltaTime);
	if (Key.down)
		Rotate(Vector2(0, -10) * m_sensitivity * m_rotateSensitivity * deltaTime);
}

void OrbitCamera::Rotate(const Vector2 &delta)
{
	Vector3 lookDir = (m_position - m_focus).Normalize();
	Vector3 camRight = lookDir.Cross(Vector3::Up()).Normalize();

	float scale = m_offset.Magnitude();

	Quaternion xRot(m_up, delta.x);
	Quaternion yRot(camRight, -delta.y);
		
	m_offset = yRot * xRot * m_offset;
	m_position = m_focus + m_offset.Scale(scale);
}

void OrbitCamera::Zoom(float delta)
{
	float currentZoom = m_offset.Magnitude();
	float newZoom = currentZoom + delta;
	m_offset.Scale(newZoom);
	m_position = m_focus + m_offset;
}

void OrbitCamera::SetPosition(const Vector3 &position)
{
	m_position = position;
	m_offset = (m_position - m_focus);
}

void OrbitCamera::SetFocus(const Vector3 &focus)
{
	m_focus = focus;
	m_offset = (m_position - m_focus);
}

void OrbitCamera::SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &up)
{
	m_position = position;
	m_focus = focus;
	m_up = up;
	m_offset = (m_position - m_focus);
}

void OrbitCamera::SetSensitivity(float sensitivity)
{
	m_sensitivity = sensitivity;
}

float OrbitCamera::GetSensitivity() const
{
	return m_sensitivity;
}

void OrbitCamera::SetZoom(float zoom)
{
	m_offset.Scale(zoom);
	m_position = m_focus + m_offset;
}

float OrbitCamera::GetZoom() const
{
	return m_offset.Magnitude();
}
