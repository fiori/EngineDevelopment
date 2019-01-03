#pragma once
#include "../Headers/objfilemodel.h"

class ModelLoader
{
private:
	ID3D11Device*			m_device_;
	ID3D11DeviceContext*	m_ImmediateContext;

	ObjFileModel*			m_Object;
	ID3D11VertexShader*		m_VShader;
	ID3D11PixelShader*		m_PShader;
	ID3D11InputLayout*		m_InputLayout;
	ID3D11Buffer*			m_ConstantBuffer;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;


public:
	ModelLoader(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext);
	~ModelLoader();

public:
	void LoadObjModel(char* fileName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
public:
	//Public methods for model modification
	//Setters
	void SetXPos(float pos) { m_x = pos; };
	void SetYPos(float pos) { m_y = pos; };
	void SetZPos(float pos) { m_z = pos; };
	void SetXRotation(float angle) { m_xAngle = angle; };
	void SetYRotation(float angle) { m_yAngle = angle; };
	void SetZRotation(float angle) { m_zAngle = angle; };
	void SetScale(float scale) { m_scale = scale; };
	//Getters
	float GetXPos() { return m_x; };
	float GetYPos() { return m_y; };
	float GetZPos() { return m_z; };
	float GetXRotation() { return m_xAngle; };
	float GetYRotation() { return m_yAngle; };
	float GetZRotation() { return m_zAngle; };
	float GetScale() { return m_scale; };
	//Modifiers
	void IncXPos(float pos) { m_x += pos; };
	void IncYPos(float pos) { m_y += pos; };
	void IncZPos(float pos) { m_z += pos; };
	void IncXRotation(float angle) { m_xAngle += angle; };
	void IncYRotation(float angle) { m_yAngle += angle; };
	void IncZRotation(float angle) { m_zAngle += angle; };
	void IncScale(float scale) { m_scale += scale; };
};

