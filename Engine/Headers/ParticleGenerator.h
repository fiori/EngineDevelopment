#pragma once
#include "../Headers/objfilemodel.h"
#include <list>

struct Particle
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT4 color;
	float age;
};

class ParticleGenerator
{
private:
	ID3D11Device*			m_device_;
	ID3D11DeviceContext*	m_ImmediateContext;

	ObjFileModel*			m_Object;
	ID3D11VertexShader*		m_VShader;
	ID3D11PixelShader*		m_PShader;
	ID3D11InputLayout*		m_InputLayout;
	ID3D11Buffer*			m_ConstantBuffer;
	ID3D11Buffer*			m_VertexBuffer;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	
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
	bool CheckCollision(ParticleGenerator* model);
	

private:
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	float GetBoundingSphereRadius();
//------------------------------------------------------------------------------------------------------------
//Particles
public:
	ParticleGenerator(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z);
	int CreateParticle();
	void DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition);
	void DrawParticle(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition);
	
	~ParticleGenerator();
private:
	float m_timePrevious;
	float m_untilParticle;
	float RandomZeroToOne();
	float RandomNegOneToPosOne();
	list<Particle*> m_free;
	list<Particle*> m_active;
	list<Particle*>::iterator it;
	float m_age;

	
//------------------------------------------------------------------------------------------------------------
public:
	void AddTexture(char* fileName);
//--------------------------------------------------------------------------------------------------------
public:
private:
	//SkyBox
	ID3D11ShaderResourceView*		m_ParticleTextureMap;
	ID3D11SamplerState*				m_ParticleSampler;
	ID3D11RasterizerState*			m_ParticleRasterSolid = 0;
	ID3D11RasterizerState*			m_ParticleRasterParticle = 0;
	ID3D11DepthStencilState*		m_ParticleDepthWriteSolid = 0;
	ID3D11DepthStencilState*		m_ParticleDepthWriteParticle = 0;
//--------------------------------------------------------------------------------------------------------

public:
	//Public methods for model modification
	//Setters
	void SetXPos(float pos) { m_x = pos; };
	void SetYPos(float pos) { m_y = pos; };
	void SetZPos(float pos) { m_z = pos; };
	void SetPosition(XMFLOAT3 Position)
	{
		m_x = Position.x; 
		m_y = Position.y; 
		m_z = Position.z;
	};
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
	//LookAt
	void LookAt_XZ(float x, float z);
	void MoveForward(float distance);
};

