#include "../Headers/ReflectModelLoader.h"

struct REFLECT_MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMMATRIX worldViewMatrix;
	XMVECTOR directional_light_vector; // 16 bytes
	XMVECTOR directional_light_color; // 16 bytes
	XMVECTOR ambient_light_color; // 16 bytes
}; REFLECT_MODEL_CONSTANT_BUFFER model_cb_values;

void ReflectModelLoader::SetDirectionalLightShinesFrom(float x, float y, float z)
{
	m_directional_light_shines_from = XMVectorSet(x, y, z, 0.0f);
}

void ReflectModelLoader::SetDirectionalLightColor(float x, float y, float z)
{
	m_directional_light_color = XMVectorSet(x, y, z, 0.0f);
}

void ReflectModelLoader::SetAmbientLightColor(float x, float y, float z)
{
	m_ambient_light_color = XMVectorSet(x, y, z, 1.0f);
}

bool ReflectModelLoader::CheckCollision(ReflectModelLoader* model)
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

	float distance_squared = pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2);
	return distance_squared < pow(this->GetBoundingSphereRadius() + model->GetBoundingSphereRadius(), 2);
}

void ReflectModelLoader::CalculateModelCentrePoint()
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

void ReflectModelLoader::CalculateBoundingSphereRadius()
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

XMVECTOR ReflectModelLoader::GetBoundingSphereWorldSpacePosition()
{
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);
	XMVECTOR offset = XMVectorSet(m_bouding_sphere_centre_x, m_bouding_sphere_centre_y, m_bouding_sphere_centre_z, 0.0f);
	offset = XMVector3Transform(offset, world);
	return offset;
}

float ReflectModelLoader::GetBoundingSphereRadius()
{
	return m_bouding_sphere_radius * m_scale;
}

ReflectModelLoader::ReflectModelLoader(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z)
	:m_device_(Device), m_ImmediateContext(ImmediateContext), m_x(x), m_y(y), m_z(z), m_xAngle(0.0f), m_yAngle(0.0f),
	m_zAngle(0.0f), m_scale(1.0f)
{
}

ReflectModelLoader::~ReflectModelLoader()
{
	if (m_textureMap)m_textureMap->Release();
	if (m_Sampler)m_Sampler->Release();
	if (m_SkyBoxTextureMap)m_SkyBoxTextureMap->Release();
	if (m_SkyBoxSampler)m_SkyBoxSampler->Release();
	if (m_SkyBoxRasterSolid)m_SkyBoxRasterSolid->Release();
	if (m_SkyBoxRasterSkyBox)m_SkyBoxRasterSkyBox->Release();
	if (m_SkyBoxDepthWriteSolid)m_SkyBoxDepthWriteSolid->Release();
	if (m_SkyBoxDepthWriteSkybox)m_SkyBoxDepthWriteSkybox->Release();
	delete m_Object;
}

void ReflectModelLoader::LoadObjModel(char* fileName)
{
	isSkyBox = false;
	m_Object = new ObjFileModel(fileName, m_device_, m_ImmediateContext);
	if (m_Object->filename == "FILE NOT LOADED")
		MessageBox(nullptr, L"Model not Loaded", nullptr, 0);

	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

	ID3DBlob *VS, *PS, *error;

	if (FAILED(D3DX11CompileFromFile(L"Shaders/reflect_shader.hlsl", nullptr, nullptr, "ModelVS", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
		MessageBox(nullptr, L"Error compiling the model vertex shader", nullptr, 0);

	//if (error != nullptr)//Check for shader compilation errors
	//{
	//	OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
	//	error->Release();
	//}
	//Create vertexShader object
	if (FAILED(m_device_->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &m_VShader)))
		MessageBox(nullptr, L"Error Creating Vertex Shader", nullptr, 0);

	//Compiling the PixelShader
	if (FAILED(D3DX11CompileFromFile(L"Shaders/reflect_shader.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
		MessageBox(nullptr, L"Error Compiling the Pixel Shader", nullptr, 0);

	//if (error != nullptr)
	//{
	//	OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
	//	error->Release();
	//}

	//Create PixelShader Object
	if (FAILED(m_device_->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &m_PShader)))
		MessageBox(nullptr, L"Error Creating the Pixel Shader", nullptr, 0);

	//Create Input Layout
	D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	if (FAILED(m_device_->CreateInputLayout(InputLayoutDesc, ARRAYSIZE(InputLayoutDesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout)))
		MessageBox(nullptr, L"Error Creating the Input Layout", nullptr, 0);

	//Create and set up the constant buffer
	D3D11_BUFFER_DESC	constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = sizeof(REFLECT_MODEL_CONSTANT_BUFFER); // Must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(m_device_->CreateBuffer(&constant_buffer_desc, nullptr, &m_ConstantBuffer)))
		MessageBox(nullptr, L"Error Creating the Constant buffer", nullptr, 0);
}

void ReflectModelLoader::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	model_cb_values.worldViewMatrix = world * (*view);
	model_cb_values.WorldViewProjection = world * (*view)*(*projection);
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &model_cb_values, 0, 0);
	m_ImmediateContext->VSSetShader(m_VShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PShader, nullptr, 0);
	m_ImmediateContext->IASetInputLayout(m_InputLayout);
	if (m_textureMap != nullptr && m_Sampler != nullptr)
	{
		m_ImmediateContext->PSSetSamplers(0, 1, &m_Sampler);
		m_ImmediateContext->PSSetShaderResources(0, 1, &m_textureMap);
	}
	if (isSkyBox)
	{
		m_ImmediateContext->RSSetState(m_SkyBoxRasterSkyBox);
		m_ImmediateContext->OMSetDepthStencilState(m_SkyBoxDepthWriteSkybox, 0);
	}
	else
	{
		m_ImmediateContext->RSSetState(m_SkyBoxRasterSolid);
		m_ImmediateContext->OMSetDepthStencilState(m_SkyBoxDepthWriteSolid, 0);
	}
	m_Object->Draw();
}

void ReflectModelLoader::LoadSkyBox(char* ObjectFile, char* FileForTheTexture)
{
	isSkyBox = true;
	m_Object = new ObjFileModel(ObjectFile, m_device_, m_ImmediateContext);
	if (m_Object->filename == "FILE NOT LOADED")
		MessageBox(nullptr, L"Model not Loaded", nullptr, 0);

	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

	ID3DBlob *VS, *PS, *error;

	if (FAILED(D3DX11CompileFromFile(L"Shaders/reflect_shader.hlsl", nullptr, nullptr, "ModelVS", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
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
	if (FAILED(D3DX11CompileFromFile(L"Shaders/reflect_shader.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
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
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	if (FAILED(m_device_->CreateInputLayout(InputLayoutDesc, ARRAYSIZE(InputLayoutDesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout)))
		MessageBox(nullptr, L"Error Creating the Input Layout", nullptr, 0);

	//Create and set up the constant buffer
	D3D11_BUFFER_DESC	constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = sizeof(REFLECT_MODEL_CONSTANT_BUFFER); // Must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(m_device_->CreateBuffer(&constant_buffer_desc, nullptr, &m_ConstantBuffer)))
		MessageBox(nullptr, L"Error Creating the Constant buffer", nullptr, 0);

	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	if (FAILED(m_device_->CreateRasterizerState(&rasterizer_desc, &m_SkyBoxRasterSolid)))
		MessageBox(nullptr, L"Errpr creatomg the solid rasterizer state", L"Error", 0); //Sucess
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	if (FAILED(m_device_->CreateRasterizerState(&rasterizer_desc, &m_SkyBoxRasterSkyBox)))
		MessageBox(nullptr, L"Errpr creating the Skybox rasterizer state", L"Error", 0); //Sucess

	D3D11_DEPTH_STENCIL_DESC DSDecsc;
	ZeroMemory(&DSDecsc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DSDecsc.DepthEnable = true;
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDecsc.DepthFunc = D3D11_COMPARISON_LESS;
	if (FAILED(m_device_->CreateDepthStencilState(&DSDecsc, &m_SkyBoxDepthWriteSolid)))
		MessageBox(nullptr, L"Error Creating the DepthWriteSolid", L"Error", 0);
	DSDecsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	if (FAILED(m_device_->CreateDepthStencilState(&DSDecsc, &m_SkyBoxDepthWriteSkybox)))
		MessageBox(nullptr, L"Error Creating the DepthWriteSkybox", L"Error", 0);

	/////////////////////////////////
	//AddTexture
	D3DX11CreateShaderResourceViewFromFileA(
		m_device_,
		FileForTheTexture,
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

	//typedef struct D3D11_RASTERIZER_DESC {
	//	D3D11_FILL_MODE FillMode;
	//	D3D11_CULL_MODE CullMode;
	//	BOOL            FrontCounterClockwise;
	//	INT             DepthBias;
	//	FLOAT           DepthBiasClamp;
	//	FLOAT           SlopeScaledDepthBias;
	//	BOOL            DepthClipEnable;
	//	BOOL            ScissorEnable;
	//	BOOL            MultisampleEnable;
	//	BOOL            AntialiasedLineEnable;
	//}
}

void ReflectModelLoader::TransposeLight()
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Tutorial 09 Lighting

	m_transpose = XMMatrixTranspose(world);
	model_cb_values.directional_light_color = m_directional_light_color;
	model_cb_values.ambient_light_color = m_ambient_light_color;
	model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, m_transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void ReflectModelLoader::LookAt_XZ(float x, float z)
{
	m_yAngle = atan2((this->m_x - x), (this->m_z - z)) * (180.0 / XM_PI);
}

void ReflectModelLoader::MoveForward(float distance)
{
	this->m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	this->m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}

void ReflectModelLoader::AddTexture(char* fileName)
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