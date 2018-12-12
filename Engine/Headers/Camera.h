#pragma once
#include <d3d11.h>
#include <dxgi.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

class Camera
{
public:
	Camera(float x, float y, float z, double CameraRotation);
	void Rotate(double degrees);
	void Forward(float distance);
	void Strafe(float distance);
	void Up(float y);
	XMMATRIX GetViewMatrix();
	//TODO: Implement Strafe
	~Camera();
private:
	float m_X;
	float m_Y;
	float m_Z;
	float m_dx;
	float m_dz;
	double m_camera_rotation;
	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;
};
