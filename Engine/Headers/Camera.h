#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi.h>

class Camera
{
public:
	Camera(float x, float y, float z, double CameraRotation);
	void Rotate(double degrees);
	void Forward(float distance);
	void Up(float y);
	DirectX::XMMATRIX GetViewMatrix();
	//TODO: Implement Strafe
	~Camera();
private:
	float m_X;
	float m_Y;
	float m_Z;
	float m_dx;
	float m_dz;
	double m_camera_rotation;
	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_lookat;
	DirectX::XMVECTOR m_up;
};
