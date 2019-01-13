#pragma once
#include "../Headers/objfilemodel.h"
#include <list>

enum type
{
	RAINBOW_FOUNTAIN,
	RAIN
};

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

	ID3D11VertexShader*		m_VShader;
	ID3D11PixelShader*		m_PShader;
	ID3D11InputLayout*		m_InputLayout;
	ID3D11Buffer*			m_ConstantBuffer;
	ID3D11Buffer*			m_VertexBuffer;
	

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	bool m_isActive;
	
//Particles
public:
	ParticleGenerator(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z);
	int CreateParticle();
	void Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition, float deltaTime);
	
	~ParticleGenerator();
private:
	float m_timePrevious;
	float m_untilParticle;
	float RandomZeroToOne();
	float RandomNegOneToPosOne();
	float RandomNumber(int min, int max);
	list<Particle*> m_free;
	list<Particle*> m_active;
	list<Particle*>::iterator it;
	float m_age;
	type m_type;
	
//--------------------------------------------------------------------------------------------------------
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
	void setActive(bool type) { m_isActive = type; };
	//Getters
	float GetXPos() const { return m_x; };
	float GetYPos() const { return m_y; };
	float GetZPos() const { return m_z; };
	float GetXRotation() const { return m_xAngle; };
	float GetYRotation() const { return m_yAngle; };
	float GetZRotation() const { return m_zAngle; };
	float GetScale() const { return m_scale; };
	bool  isActive() const { return m_isActive; };
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

