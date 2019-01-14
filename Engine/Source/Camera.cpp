#include "../Headers/Camera.h"
#include <xnamath.h>
#include <thread>
#include "../Headers/Defines.h"

Camera::Camera(float x, float y, float z, double CameraRotation, float pitch)
	:m_X(x), m_Y(y), m_Z(z), m_camera_rotation(CameraRotation), m_pitch(pitch)
{
	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
	m_dy = sin(m_pitch * (XM_PI / 180));
}

void Camera::Rotate(double degrees)
{
	m_camera_rotation += degrees;
	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
	m_Y += m_velocityY;

}

void Camera::Forward(float distance)
{
	XMVECTOR const forward = XMVector4Normalize(m_lookat - m_position);
	m_X += (forward.x * distance);
	m_Z += (forward.z * distance);
}

void Camera::Pitch(float degrees)
{
	m_pitch += degrees;
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;
	m_dy = sin(m_pitch * (XM_PI / 180));
}



void Camera::Strafe(float distance)
{
	XMVECTOR const forward = XMVector4Normalize(m_lookat - m_position);
	XMVECTOR const right = XMVector3Cross(forward, m_up);

	m_X += (right.x * distance);
	m_Z += (right.z * distance);
}

void Camera::Up(float y)
{
	m_Y += y;
}

void Camera::Jump(float deltaTime)
{
	if (m_Y == 0)
	{
		m_velocityY = 15.0f * deltaTime;
		m_jumping = true;
	}
}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_X, m_Y, m_Z, 0);
	m_lookat = XMVectorSet(m_X + m_dx, m_Y + m_dy, m_Z + m_dz, 0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

Camera::~Camera()
{
}