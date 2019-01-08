#include "../Headers/ModelLoader.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector; // 16 bytes
	XMVECTOR directional_light_color; // 16 bytes
	XMVECTOR ambient_light_color; // 16 bytes
};MODEL_CONSTANT_BUFFER model_cb_values;

void ModelLoader::SetDirectionalLightShinesFrom(float x, float y, float z)
{
	m_directional_light_shines_from = XMVectorSet(x, y, z, 0.0f);
}

void ModelLoader::SetDirectionalLightColor(float x, float y, float z)
{
	m_directional_light_color = XMVectorSet(x, y, z, 0.0f);
}

void ModelLoader::SetAmbientLightColor(float x, float y, float z)
{
	m_ambient_light_color = XMVectorSet(x, y, z, 1.0f);
}

ModelLoader::ModelLoader(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, float x, float y, float z)
	:m_device_(Device), m_ImmediateContext(ImmediateContext), m_x(x), m_y(y), m_z(z), m_xAngle(0.0f), m_yAngle(0.0f),
	m_zAngle(0.0f), m_scale(1.0f)
{
	
}

ModelLoader::~ModelLoader()
{
	if (m_textureMap)m_textureMap->Release();
	if (m_Sampler)m_Sampler->Release();
	delete m_Object;
}

void ModelLoader::LoadObjModel(char* fileName)
{
	m_Object = new ObjFileModel(fileName, m_device_, m_ImmediateContext);
	if (m_Object->filename == "FILE NOT LOADED")
		MessageBox(nullptr, L"Model not Loaded", nullptr, 0);

	ID3DBlob *VS, *PS, *error;

	if (FAILED(D3DX11CompileFromFile(L"Shaders/model_shaders.hlsl", nullptr, nullptr, "ModelVS", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
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
	if (FAILED(D3DX11CompileFromFile(L"Shaders/PixelShader.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
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
	constant_buffer_desc.ByteWidth = sizeof(MODEL_CONSTANT_BUFFER); // Must be a multiple of 16
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(m_device_->CreateBuffer(&constant_buffer_desc, nullptr, &m_ConstantBuffer)))
		MessageBox(nullptr, L"Error Creating the Constant buffer", nullptr, 0);
}


void ModelLoader::Draw(XMMATRIX* view, XMMATRIX* projection)
{

	world = XMMatrixTranslation(m_x, m_y, m_z);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	model_cb_values.WorldViewProjection = world*(*view)*(*projection);
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &model_cb_values,0,0);
	m_ImmediateContext->VSSetShader(m_VShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PShader, nullptr, 0);
	m_ImmediateContext->IASetInputLayout(m_InputLayout);
	if (m_textureMap != nullptr && m_Sampler != nullptr)
	{
		m_ImmediateContext->PSSetSamplers(0, 1, &m_Sampler);
		m_ImmediateContext->PSSetShaderResources(0, 1, &m_textureMap);
	}
	m_Object->Draw();
}

void ModelLoader::TransposeLight()
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

void ModelLoader::LookAt_XZ(float x, float z)
{
	m_yAngle = atan2((this->m_x - x),(this->m_z - z)) * (180.0 / XM_PI);
}

void ModelLoader::MoveForward(float distance)
{
	this->m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	this->m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}

void ModelLoader::AddTexture(char* fileName)
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


