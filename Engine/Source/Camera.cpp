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

Camera::~Camera()
{
}