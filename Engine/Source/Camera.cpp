#include "../Headers/Camera.h"

Camera::Camera(float x, float y, float z, double CameraRotation)
	:m_X(x), m_Y(y), m_Z(z), m_camera_rotation(CameraRotation)
{
	m_dx = sin(m_camera_rotation * (DirectX::XM_PI / 180));
	m_dz = cos(m_camera_rotation * (DirectX::XM_PI / 180));
}

void Camera::Rotate(double degrees)
{
	m_camera_rotation += degrees;
	m_dx = sin(m_camera_rotation * (DirectX::XM_PI / 180));
	m_dz = cos(m_camera_rotation * (DirectX::XM_PI / 180));
}

void Camera::Forward(float distance)
{
	m_X += distance * m_dx;
	m_Y += distance * m_dz;
}

void Camera::Up(float y)
{
	m_Y += y;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	m_position = DirectX::XMVectorSet(m_X, m_Y, m_Z, 0);
	m_lookat = DirectX::XMVectorSet(m_X + m_dx, m_Y, m_Z + m_dz, 0);
	m_up = DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0);
	return DirectX::XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

Camera::~Camera()
{
}