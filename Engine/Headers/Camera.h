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
	//TODO: Implement Pitch on the camera
	~Camera();

private:
	float
		m_X,
		m_Y,
		m_Z,
		m_dx,
		m_dz;
	
	double m_camera_rotation;
	
	XMVECTOR 
		m_position,
		m_lookat,
		m_up;
};
