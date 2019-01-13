#include "../Headers/ParticleGenerator.h"
#include <algorithm>
#include <random>
#include "../Headers/Defines.h"

struct PARTICLE_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMVECTOR Color;
};PARTICLE_CONSTANT_BUFFER model_cb_values;


ParticleGenerator::ParticleGenerator(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z)
	:m_device_(Device), m_ImmediateContext(ImmediateContext), m_x(x), m_y(y), m_z(z), m_xAngle(0.0f), m_yAngle(0.0f),
	m_zAngle(0.0f), m_scale(0.1f), m_isActive(true), m_type(RAIN)
{
	for (int i = 0; i < 1000; i++)
	{
		Particle* one = new Particle();
		one->color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
		one->gravity = PARTICLE_GRAVITY;
		one->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		one->velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_free.push_back(one);
	}
}

int ParticleGenerator::CreateParticle()
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[6] =
	{
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3( 1.0f,  1.0f, 0.0f),
		XMFLOAT3(-1.0f,  1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3( 1.0f, -1.0f, 0.0f),
		XMFLOAT3( 1.0f,  1.0f, 0.0f),
	};

	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	hr = m_device_->CreateRasterizerState(&rasterizer_desc, &m_ParticleRasterSolid);
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	hr = m_device_->CreateRasterizerState(&rasterizer_desc, &m_ParticleRasterParticle);

	//create the ver buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = m_device_->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer);

	if (FAILED(hr)) return 0;

	//copy verts to buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//mapping = locking the buffer which allows writing
	m_ImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));
	//unlock the buffer
	m_ImmediateContext->Unmap(m_VertexBuffer, 0);

	//Create and set up the constant buffer
	D3D11_BUFFER_DESC	constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = sizeof(PARTICLE_CONSTANT_BUFFER); // Must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(m_device_->CreateBuffer(&constant_buffer_desc, NULL, &m_ConstantBuffer)))
		MessageBox(nullptr, L"Error Creating the Constant buffer", nullptr, 0);

	ID3DBlob *VS, *PS, *error;

	if (FAILED(D3DX11CompileFromFile(L"Shaders/ParticleGenerator.hlsl", nullptr, nullptr, "ModelVS", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
		MessageBox(nullptr, L"Error compiling the model vertex shader", nullptr, 0);

	if (error != nullptr)//Check for shader compilation errors
	{
		OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
		error->Release();
	}
	//Create vertexShader object
	if (FAILED(m_device_->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &m_VShader)))
		MessageBox(nullptr, L"Error Creating Vertex Shader", nullptr, 0);

	//Compiling the PixelShader
	if (FAILED(D3DX11CompileFromFile(L"Shaders/ParticleGenerator.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
		MessageBox(nullptr, L"Error Compiling the Pixel Shader", nullptr, 0);

	if (error != nullptr)
	{
		OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
		error->Release();
	}

	//Create PixelShader Object
	if (FAILED(m_device_->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &m_PShader)))
		MessageBox(nullptr, L"Error Creating the Pixel Shader", nullptr, 0);

	//Create Input Layout
	D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	if (FAILED(m_device_->CreateInputLayout(InputLayoutDesc, ARRAYSIZE(InputLayoutDesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout)))
		MessageBox(nullptr, L"Error Creating the Input Layout", nullptr, 0);

	m_ImmediateContext->IASetInputLayout(m_InputLayout);

	return 0;
}

void ParticleGenerator::Draw(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition, float deltaTime)
{
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;

	/*Particle one;
	one.color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
	one.gravity = 0.0f;
	one.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	one.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
*/
	m_untilParticle -= deltaTime;

	if (m_untilParticle <= 0.0f)
	{
		if (m_isActive)
		{
			it = m_free.begin();//point to the beggining of the free list
			//add a new particle to the back of m_active from the front of m_free
			if (m_free.size() != NULL)
			{
				switch (m_type)
				{
				case RAINBOW_FOUNTAIN:
					{
						m_age = PARTICLE_AGE;
						m_untilParticle = PARTICLE_TIME;
						(*it)->color = XMFLOAT4(RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne(), 1.0f);
						(*it)->gravity = 4.5f;
						(*it)->position = XMFLOAT3(0.0f, 6.0f, 3.0f);
						(*it)->velocity = XMFLOAT3(RandomNegOneToPosOne(), 2.50f, RandomNegOneToPosOne());
						break;
					}				
				case RAIN:
				{
					m_age = PARTICLE_AGE;
					m_untilParticle = PARTICLE_TIME;
					(*it)->color = XMFLOAT4(RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne(), 1.0f);
					(*it)->gravity = PARTICLE_GRAVITY;
					(*it)->position = XMFLOAT3(this->m_x + RandomNumber(50,100), 80.0f, this->m_z + RandomNumber(50,100));
					(*it)->velocity = XMFLOAT3(RandomNegOneToPosOne(), -10.0f, RandomNegOneToPosOne());
					break;
				}
				default:
					break;
				}
				(*it)->age = 0.0f;
				//Add the particle from the front of the available list to the back of the active list and
				//remove it
				m_active.push_back(*it);
				m_free.pop_front();
			}


		}
		else
			m_untilParticle = PARTICLE_TIME;
	}
	if (m_active.size() != NULL)
	{
		it = m_active.begin();
		while (it != m_active.end())
		{
			switch (m_type)
			{
			case RAINBOW_FOUNTAIN:
			{
				(*it)->age += deltaTime;
				(*it)->velocity.y -= (*it)->gravity*(deltaTime);
				(*it)->position.x += (*it)->velocity.x*(deltaTime);
				(*it)->position.y += (*it)->velocity.y*(deltaTime);
				(*it)->position.z += (*it)->velocity.z*(deltaTime);
				break;
			}			
			case RAIN:
			{
				(*it)->age += deltaTime;
				(*it)->velocity.y -= (*it)->gravity*(deltaTime);
				(*it)->position.x += (*it)->velocity.x*(deltaTime);
				(*it)->position.y += (*it)->velocity.y*(deltaTime);
				(*it)->position.z += (*it)->velocity.z*(deltaTime);
				break;
			}
			default:
				break;
			}
			XMMATRIX local_world;

			local_world = XMMatrixIdentity();
			local_world *= XMMatrixScaling(m_scale, m_scale, m_scale);
			local_world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
			local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
			local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
			local_world *= XMMatrixTranslation((*it)->position.x, (*it)->position.y, (*it)->position.z);


			//constant buffer stuff for shader
			model_cb_values.WorldViewProjection = local_world * (*view)*(*projection);
			model_cb_values.Color = XMVectorSet((*it)->color.x, (*it)->color.y, (*it)->color.z, 0.0f);
			m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
			m_ImmediateContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
			m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, 0, &model_cb_values, 0, 0);
			m_ImmediateContext->VSSetShader(m_VShader, 0, 0);
			m_ImmediateContext->PSSetShader(m_PShader, 0, 0);
			m_ImmediateContext->IASetInputLayout(m_InputLayout);
			m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
			m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_ImmediateContext->RSSetState(m_ParticleRasterParticle);
			m_ImmediateContext->Draw(6, 0);
			m_ImmediateContext->RSSetState(m_ParticleRasterSolid);

			if ((*it)->age >= m_age)//check the age of the current particle
			{
				it++;
				m_active.front()->age = m_age;
				m_active.front()->position = { m_x, m_y, m_z };
				m_active.front()->velocity = { 0.0f,4.50f,RandomNegOneToPosOne() };
				m_free.push_back(m_active.front());
				m_active.pop_front();
			}
			else
				it++;
		}
	}

}

ParticleGenerator::~ParticleGenerator()
{
	for (Particle* element : m_free)
	{
		element = nullptr;
		delete element;
	}
	/*if (m_textureMap)m_textureMap->Release();
	if (m_Sampler)m_Sampler->Release();*/
	if(m_ParticleTextureMap)m_ParticleTextureMap->Release();
	if(m_ParticleSampler)m_ParticleSampler->Release();
	if (m_ParticleRasterSolid)m_ParticleRasterSolid->Release();
	if (m_ParticleRasterParticle)m_ParticleRasterParticle->Release();
	if (m_ParticleDepthWriteSolid)m_ParticleDepthWriteSolid->Release();
	if (m_ParticleDepthWriteParticle)m_ParticleDepthWriteParticle->Release();
}

float ParticleGenerator::RandomZeroToOne()
{
	int temp = rand();
	return ((temp / 32767.0f) * 2) - 1;
}

float ParticleGenerator::RandomNegOneToPosOne()
{
	int temp = rand();
	return ((temp / 32767.0f));
}
float ParticleGenerator::RandomNumber(int min, int max)
{
	int temp = rand();
	return ((temp / 32767.0f) * max) - min;
}


void ParticleGenerator::LookAt_XZ(float x, float z)
{
	m_yAngle = atan2((this->m_x - x),(this->m_z - z)) * (180.0 / XM_PI);
}

void ParticleGenerator::MoveForward(float distance)
{
	this->m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	this->m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}



