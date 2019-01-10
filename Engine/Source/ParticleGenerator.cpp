#include "../Headers/ParticleGenerator.h"
#include <algorithm>
#include <random>

struct PARTICLE_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMFLOAT4 Color;
};PARTICLE_CONSTANT_BUFFER model_cb_values;

void ParticleGenerator::SetDirectionalLightShinesFrom(float x, float y, float z)
{
	m_directional_light_shines_from = XMVectorSet(x, y, z, 0.0f);
}

void ParticleGenerator::SetDirectionalLightColor(float x, float y, float z)
{
	m_directional_light_color = XMVectorSet(x, y, z, 0.0f);
}

void ParticleGenerator::SetAmbientLightColor(float x, float y, float z)
{
	m_ambient_light_color = XMVectorSet(x, y, z, 1.0f);
}

bool ParticleGenerator::CheckCollision(ParticleGenerator* model)
{
	if (model == this)
	{
		MessageBox(nullptr, L"Cannot collide with the same object", nullptr, 0);
		return false;
	}

	float x1 = XMVectorGetX(this->GetBoundingSphereWorldSpacePosition());
	float x2 = XMVectorGetX(model->GetBoundingSphereWorldSpacePosition());
	float y1 = XMVectorGetY(this->GetBoundingSphereWorldSpacePosition());
	float y2 = XMVectorGetY(model->GetBoundingSphereWorldSpacePosition());
	float z1 = XMVectorGetZ(this->GetBoundingSphereWorldSpacePosition());
	float z2 = XMVectorGetZ(model->GetBoundingSphereWorldSpacePosition());
	
	float distance_squared = pow(x1 - x2,2) + pow(y1 - y2, 2) + pow(z1 - z2,2);
	return distance_squared < pow(this->GetBoundingSphereRadius() + model->GetBoundingSphereRadius(),2);
	
}

void ParticleGenerator::CalculateModelCentrePoint()
{
	m_MinimumVertPos = m_Object->vertices[0].Pos;
	m_MaximumVertPos = m_Object->vertices[0].Pos;

	for (int i = 0; i < m_Object->numverts; i++)
	{
		if (m_Object->vertices[i].Pos.x < m_MinimumVertPos.x)
		{
			m_MinimumVertPos.x = m_Object->vertices[i].Pos.x;
		}	
		if (m_Object->vertices[i].Pos.y < m_MinimumVertPos.y)
		{
			m_MinimumVertPos.y = m_Object->vertices[i].Pos.y;
		}	
		if (m_Object->vertices[i].Pos.z < m_MinimumVertPos.z)
		{
			m_MinimumVertPos.z = m_Object->vertices[i].Pos.z;
		}		
		if (m_Object->vertices[i].Pos.x > m_MaximumVertPos.x)
		{
			m_MaximumVertPos.x = m_Object->vertices[i].Pos.x;
		}		
		if (m_Object->vertices[i].Pos.y > m_MaximumVertPos.y)
		{
			m_MaximumVertPos.y = m_Object->vertices[i].Pos.y;
		}		
		if (m_Object->vertices[i].Pos.z > m_MaximumVertPos.z)
		{
			m_MaximumVertPos.z = m_Object->vertices[i].Pos.z;
		}	
		
		
		m_bouding_sphere_centre_x = ((m_MaximumVertPos.x + m_MinimumVertPos.x) / 2);
		m_bouding_sphere_centre_y = ((m_MaximumVertPos.y + m_MinimumVertPos.y) / 2);
		m_bouding_sphere_centre_z = ((m_MaximumVertPos.z + m_MinimumVertPos.z) / 2);
	}
}

void ParticleGenerator::CalculateBoundingSphereRadius()
{
	float distance_squared = 0.0f;
	m_bouding_sphere_radius = sqrt(pow(m_bouding_sphere_centre_x, 2) + pow(m_bouding_sphere_centre_y, 2) + pow(m_bouding_sphere_centre_z, 2));

	for (int i = 0; i < m_Object->numverts; i++)
	{
		distance_squared = sqrt(pow(m_Object->vertices[i].Pos.x - m_bouding_sphere_centre_x, 2) +
			pow(m_Object->vertices[i].Pos.y - m_bouding_sphere_centre_y, 2) +
			pow(m_Object->vertices[i].Pos.z - m_bouding_sphere_centre_z, 2));

		if (distance_squared > m_bouding_sphere_radius)
		{
			m_bouding_sphere_radius = distance_squared;
		}
	}
}

XMVECTOR ParticleGenerator::GetBoundingSphereWorldSpacePosition()
{
	world = XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);
	XMVECTOR offset = XMVectorSet(m_bouding_sphere_centre_x,m_bouding_sphere_centre_y,m_bouding_sphere_centre_z, 0.0f);
	offset = XMVector3Transform(offset, world);
	return offset;
}

float ParticleGenerator::GetBoundingSphereRadius()
{
	return m_bouding_sphere_radius * m_scale;
}


ParticleGenerator::ParticleGenerator(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z)
	:m_device_(Device), m_ImmediateContext(ImmediateContext), m_x(x), m_y(y), m_z(z), m_xAngle(0.0f), m_yAngle(0.0f),
	m_zAngle(0.0f), m_scale(1.0f)
{
	for (int i = 0; i < 100; i++)
	{
		Particle* test = new Particle[i]();
		m_free.push_back(test);
	}
}

int ParticleGenerator::CreateParticle()
{
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
	m_device_->CreateRasterizerState(&rasterizer_desc, &m_ParticleRasterSolid);
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	m_device_->CreateRasterizerState(&rasterizer_desc, &m_ParticleRasterParticle);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * 6;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_device_->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer);

	D3D11_MAPPED_SUBRESOURCE ms;

	m_ImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));
	m_ImmediateContext->Unmap(m_VertexBuffer, 0);

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

	//Create and set up the constant buffer
	D3D11_BUFFER_DESC	constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = sizeof(PARTICLE_CONSTANT_BUFFER); // Must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(m_device_->CreateBuffer(&constant_buffer_desc, nullptr, &m_ConstantBuffer)))
		MessageBox(nullptr, L"Error Creating the Constant buffer", nullptr, 0);

	return 0;

}

void ParticleGenerator::DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition)
{
	//UINT stride = sizeof(XMFLOAT3);

	//float timeNow = float(timeGetTime()) / 1000.0f;
	//float deltaTime = timeNow - m_timePrevious;
	//m_timePrevious = timeNow;
	//m_untilParticle -= deltaTime;

	//if (m_untilParticle <= 0.0f)
	//{
	//	it = m_free.begin();
	//	if (m_free.size() != NULL)
	//	{
	//		m_age = 2.0f;
	//		m_untilParticle = 0.008f;
	//		(*it)->color = XMFLOAT4(RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne(), 1.0f);
	//		(*it)->gravity = 4.5f;
	//		(*it)->position= XMFLOAT3(0.0f,1.0f,3.0f);
	//		(*it)->velocity = XMFLOAT3(RandomZeroToOne(), 2.50f, RandomZeroToOne());
	//		(*it)->age = 0.0f;

	//		m_active.push_back(*it);
	//		m_free.pop_front();

	//	}
	//	m_untilParticle = 2.0f;
	//}

	//if(m_active.size() != NULL)
	//{
	//	it = m_active.begin();
	//	while(it != m_active.end())
	//	{
	//		(*it)->age += deltaTime;
	//		(*it)->velocity.y -= (*it)->gravity*(deltaTime);
	//		(*it)->position.x += (*it)->velocity.x*(deltaTime);
	//		(*it)->position.y += (*it)->velocity.y*(deltaTime);
	//		(*it)->position.z += (*it)->velocity.z*(deltaTime);
	//	}
	//}

	world = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	world *= XMMatrixRotationY(XMConvertToRadians(XM_PI));
	world *= XMMatrixTranslation(one->position.x, one->position.y, one->position.z);

	model_cb_values.Color = one->color;
	model_cb_values.WorldViewProjection = world * (*view)*(*projection);
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &model_cb_values, 0, 0);
	m_ImmediateContext->VSSetShader(m_VShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PShader, nullptr, 0);
	m_ImmediateContext->IASetInputLayout(m_InputLayout);
	m_ImmediateContext->Draw(6, 0);
}

void ParticleGenerator::DrawParticle(XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* cameraposition)
{
	m_timePrevious = (float)timeGetTime();

	Particle test;
	test.color = XMFLOAT4(1.0f, 0.0f, 0.3f, 1.0f);
	test.gravity = 1;
	test.position = XMFLOAT3(0.0f, 3.0f, 14.0f);
	test.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	DrawOne(&test, view, projection, cameraposition);


}

ParticleGenerator::~ParticleGenerator()
{
	for (Particle* element : m_free)
	{
		element = nullptr;
		delete element;
	}
	if (m_textureMap)m_textureMap->Release();
	if (m_Sampler)m_Sampler->Release();
	if(m_ParticleTextureMap)m_ParticleTextureMap->Release();
	if(m_ParticleSampler)m_ParticleSampler->Release();
	if (m_ParticleRasterSolid)m_ParticleRasterSolid->Release();
	if (m_ParticleRasterParticle)m_ParticleRasterParticle->Release();
	if (m_ParticleDepthWriteSolid)m_ParticleDepthWriteSolid->Release();
	if (m_ParticleDepthWriteParticle)m_ParticleDepthWriteParticle->Release();
	delete m_Object;
}

float ParticleGenerator::RandomZeroToOne()
{
	
	std::mt19937 rng(0);
	uniform_int_distribution<int> gen(0, 1); // uniform, unbiased

	return (float)gen(rng);
}

float ParticleGenerator::RandomNegOneToPosOne()
{
	std::mt19937 rng(0);
	uniform_int_distribution<int> gen(-1, 1); // uniform, unbiased

	return (float)gen(rng);
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

void ParticleGenerator::AddTexture(char* fileName)
{
	D3DX11CreateShaderResourceViewFromFileA(
		m_device_,
		fileName,
		nullptr,
		nullptr,
		&m_textureMap,
		nullptr);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_device_->CreateSamplerState(&sampler_desc, &m_Sampler);
}


