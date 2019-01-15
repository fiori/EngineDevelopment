#pragma once
#include "objfilemodel.h"
#include "Transform.h"

class ReflectModelLoader : public Transform
{
private:
	ID3D11Device*			m_device_;
	ID3D11DeviceContext*	m_ImmediateContext;

	ObjFileModel*			m_Object;
	ID3D11VertexShader*		m_VShader;
	ID3D11PixelShader*		m_PShader;
	ID3D11InputLayout*		m_InputLayout;
	ID3D11Buffer*			m_ConstantBuffer;

private:
	XMMATRIX world;
private:
	//Light Matrix
	XMMATRIX m_transpose;
	//Light Vectors with default values
	XMVECTOR m_directional_light_shines_from = XMVectorSet(1.0f, 1.0f, -1.0f, 1.0f);
	XMVECTOR m_directional_light_color = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); // White
	XMVECTOR m_ambient_light_color = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // Dark Grey - always use a small value for ambient lighting
public:
	//Getters
	XMVECTOR GetDirectionalLightShinesFrom() const { return m_directional_light_shines_from; };
	XMVECTOR GetDirectionalLightColor() const { return m_directional_light_color; };
	XMVECTOR GetAmbientLightColor() const { return m_ambient_light_color; };
	//Setters
	void SetDirectionalLightShinesFrom(float x, float y, float z);
	void SetDirectionalLightColor(float x, float y, float z);
	void SetAmbientLightColor(float x, float y, float z);
private:
	//Texture
	ID3D11ShaderResourceView*	m_textureMap;
	ID3D11SamplerState*			m_Sampler;

	//Tutorial12
private:
	float m_bouding_sphere_centre_x, m_bouding_sphere_centre_y, m_bouding_sphere_centre_z, m_bouding_sphere_radius;
	XMFLOAT3 m_MinimumVertPos, m_MaximumVertPos;
public:
	bool CheckCollision(ReflectModelLoader* model);

private:
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRadius();
	//--------------------------------
public:
	ReflectModelLoader(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z);
	~ReflectModelLoader();

public:
	void LoadObjModel(char* fileName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void AddTexture(char* fileName);
	void TransposeLight();
	//--------------------------------------------------------------------------------------------------------
public:
	//SkyBox
	void LoadSkyBox(char* ObjectFile, char* FileForTheTexture);
private:
	//SkyBox
	bool isSkyBox = false;
	ID3D11ShaderResourceView*		m_SkyBoxTextureMap;
	ID3D11SamplerState*				m_SkyBoxSampler;
	ID3D11RasterizerState*			m_SkyBoxRasterSolid = 0;
	ID3D11RasterizerState*			m_SkyBoxRasterSkyBox = 0;
	ID3D11DepthStencilState*		m_SkyBoxDepthWriteSolid = 0;
	ID3D11DepthStencilState*		m_SkyBoxDepthWriteSkybox = 0;
	//--------------------------------------------------------------------------------------------------------

public:
	//LookAt
	void LookAt_XZ(float x, float z);
	void MoveForward(float distance);
};
