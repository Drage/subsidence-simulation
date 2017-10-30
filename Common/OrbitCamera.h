
/*
 * @file	OrbitCamera.h/.cpp
 * @brief	Camera subtype that can zoom and rotate around a pivot.
 * @details	Appropriate value for sensitivity may depend on distance to focus.
 *			Update function should be called once per frame.
 * @author	Matt Drage
 * @date	02/01/2013
 */

#ifndef ORBITCAMERA_H
#define ORBITCAMERA_H

#include "Camera.h"
#include "Vector2.h"

class OrbitCamera : public Camera
{
	public:
		// Constructors
		OrbitCamera();
		OrbitCamera(const Vector3 &position, const Vector3 &focus, float sensitivity);
	
		// Handle key/mouse input
		void Update(double deltaTime);

		// Set camera attributes
		void SetPosition(const Vector3 &position);
		void SetFocus(const Vector3 &focus);
		void SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &up = Vector3(0, 1, 0));
		void SetSensitivity(float sensitivity);
		float GetSensitivity() const;
		void SetZoom(float zoom);
		float GetZoom() const;

	private:
		void Rotate(const Vector2 &delta);
		void Zoom(float delta);

		Vector3 m_offset;
		float m_rotateSensitivity;
		float m_panSensitivity;
		float m_zoomSensitivity;
		float m_sensitivity;
};

#endif
