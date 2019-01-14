#pragma once
#include "ModelLoader.h"
#include "Camera.h"

class Weapon
{
private:
	ID3D11Device* m_device_;
	ID3D11DeviceContext* m_ImmediateContext;
	ObjFileModel* m_Gun;
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Angle;
	XMFLOAT3 m_Scale;
	ID3D11VertexShader* m_VShader;
	ID3D11PixelShader* m_PShader;
	ID3D11InputLayout* m_InputLayout;
	ID3D11Buffer* m_ConstantBuffer;
	XMMATRIX world;
	ID3D11ShaderResourceView* m_textureMap;
	ID3D11SamplerState* m_Sampler;
	//Light Matrix
	XMMATRIX m_transpose;
	//Light Vectors with default values
	XMVECTOR m_directional_light_shines_from = XMVectorSet(1.0f, 1.0f, -1.0f, 1.0f);
	XMVECTOR m_directional_light_color = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); // White
	XMVECTOR m_ambient_light_color = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // Dark Grey - always use a small value for ambient lighting

public:
	Weapon();
	~Weapon();

public:
	Weapon(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, XMFLOAT3 Rotation, float x, float y, float z);
	void LoadWeapon(char* fileName);
	void AddTexture(char* fileName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void Rotate(XMFLOAT3 degrees);

public:
	//Setter
	void SetPosition(XMFLOAT3 Position);
	void SetScale(XMFLOAT3 Scale);
	//void SetStrafe(Camera* Camera);
	//void SetPitch(Camera* Camera);
	void SetRotation(Camera* Camera);
	void TransposeLight();
	//Getters
	XMFLOAT3 GetPosition() const { return m_Position; };
	XMFLOAT3 GetRotation() const { return m_Angle; };
	XMFLOAT3 GetScale() const { return m_Scale; };
};
