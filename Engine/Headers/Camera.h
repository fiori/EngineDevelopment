#pragma once
#include <d3d11.h>
#include <dxgi.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

class Camera
{
public:
	Camera(float x, float y, float z, double CameraRotation, float pitch);
	void Rotate(double degrees);
	void Forward(float distance);
	void Strafe(float distance);
	void Pitch(float degrees);
	void Up(float y);
	void Jump(float deltaTime);
	XMMATRIX GetViewMatrix();
	~Camera();

public:
	//Getters
	float GetX() const { return m_X; };
	float GetY() const { return m_Y; };
	float GetZ() const { return m_Z; };
	float GetDX() const { return m_dx; };
	float GetDY() const { return m_dy; };
	float GetDZ() const { return m_dz; };
	float GetRotation() const { return m_camera_rotation; };
	float GetPitch(){ return -m_pitch;};
	bool getJumping() { return m_jumping; };
	XMFLOAT3 GetPosition() const { return XMFLOAT3(m_X, m_Y, m_Z); };
	XMFLOAT3 GetDPosition() const { return XMFLOAT3(m_dx, m_dy, m_dz); };
	//Setters
	void SetX(float x) { m_X = x; };
	void SetY(float y) { m_Y = y; };
	void SetZ(float z) { m_Z = z; };
	void SetJump(bool jump) { m_jumping = jump; m_velocityY = 0; };
	void IncXPos(float pos) { m_X += pos; };
	void IncYPos(float pos) { m_Y += pos; };
	void IncZPos(float pos) { m_Z += pos; };

private:
	float
		m_X,
		m_Y,
		m_Z,
		m_dx,
		m_dz,
		m_dy,
		m_pitch,
		m_velocityY = 0;

	double
		m_camera_rotation;

	bool m_jumping = false;
		

private:
	XMVECTOR
		m_position,
		m_lookat,
		m_up;
};
