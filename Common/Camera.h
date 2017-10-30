
/*
 * @file	Camera.h/.cpp
 * @brief	Defines viewpoint for perspective rendering.
 * @details	Abstracts the process of converting camera coordinates and properties into
 *			vertex manipulation matrices. Matricies are set up to be used with OpenGL
 *			function calls such as glLoadMatrixf(camera.GetViewMatrix().ToArray()).
 * @author	Matt Drage
 * @date	20/12/2012
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"
#include "Matrix4x4.h"

class Camera
{
	public:
		// Constructors
		Camera();
		Camera(const Vector3 &position, const Vector3 &focus, const Vector3 &up = Vector3(0, 1, 0), float fov = 60, float nearClip = 0.1f, float farClip = 1000.0f);
		virtual ~Camera() {}

		// Default update function - may be overloaded in derived classes
		virtual void Update(double deltaTime) {}

		// View matrix modifiers
		virtual void SetPosition(const Vector3 &position);
		virtual void SetFocus(const Vector3 &focus);
		void SetUpVector(const Vector3 &up);
		virtual void SetView(const Vector3 &position, const Vector3 &focus, const Vector3 &up = Vector3(0, 1, 0));
	
		// Projection matrix modifiers
		void SetFOV(float fov);
		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);
		void SetProjectionPerspective(float fov, float nearClip, float farClip);

		// Get view attributes
		Vector3 GetPosition() const;
		Vector3 GetFocus() const;
		Vector3 GetUpVector() const;

		// Get projection attributes
		float GetFOV() const;
		float GetNearClip() const;
		float GetFarClip() const;

		// Get vertex manipulation matrices
		const Matrix4x4& GetViewMatrix();
		const Matrix4x4& GetProjectionMatrix();

	protected:
		Vector3 m_position;
		Vector3 m_focus;
		Vector3 m_up;
		float m_fov;
		float m_nearClip;
		float m_farClip;
		Matrix4x4 m_viewMatrix;
		Matrix4x4 m_projectionMatrix;
};

#endif
