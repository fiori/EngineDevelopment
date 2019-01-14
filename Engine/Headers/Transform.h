#pragma once
#include "scene_node.h"

class Transform
{
protected:
	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	float m_xScale;
	float m_zScale;

	public:
	//Public methods for model modification
	//Setters
	virtual void SetXPos(float pos) { m_x = pos; };
	virtual void SetYPos(float pos) { m_y = pos; };
	virtual void SetZPos(float pos) { m_z = pos; };
	virtual void SetPosition(XMFLOAT3 Position)
	{
		m_x = Position.x;
		m_y = Position.y;
		m_z = Position.z;
	};
	virtual void SetXRotation(float angle) { m_xAngle = angle; };
	virtual void SetYRotation(float angle) { m_yAngle = angle; };
	virtual void SetZRotation(float angle) { m_zAngle = angle; };
	virtual void SetScale(float scale) { m_scale = scale; };
	virtual void SetXZScale(float X, float Z) { m_xScale = X; m_zScale = Z; };

	//Getters
	virtual float GetXPos() const { return m_x; };
	virtual float GetYPos() const { return m_y; };
	virtual float GetZPos() const { return m_z; };
	virtual XMVECTOR GetPosition() const { return XMVectorSet(m_x, m_y, m_z, 0.0f); };
	virtual float GetXRotation() const { return m_xAngle; };
	virtual float GetYRotation() const { return m_yAngle; };
	virtual float GetZRotation() const { return m_zAngle; };
	virtual float GetScale() const { return m_scale; };
	//Modifiers
	virtual void IncXPos(float pos) { m_x += pos; };
	virtual void IncYPos(float pos) { m_y += pos; };
	virtual void IncZPos(float pos) { m_z += pos; };
	virtual void IncXRotation(float angle) { m_xAngle += angle; };
	virtual void IncYRotation(float angle) { m_yAngle += angle; };
	virtual void IncZRotation(float angle) { m_zAngle += angle; };
	virtual void IncScale(float scale) { m_scale += scale; };
};

