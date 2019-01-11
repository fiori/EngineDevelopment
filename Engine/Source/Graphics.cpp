#include "../Headers/Graphics.h"
#include "../Headers/window.h"
#include <smmintrin.h>

Graphics::Graphics(Window& window)
	:wnd(window)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::SCREENWIDTH;
	sd.BufferDesc.Height = Graphics::SCREENHEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = wnd.m_MainWnd;

	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	UINT CreateDeviceFlags = 0u;

	//#if defined(DEBUG) || defined(_DEBUG)
	//	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//#endif

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		CreateDeviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_SwapChain,
		&m_Device,
		nullptr,
		&m_ImmediateContext)
	))
		MessageBox(wnd.m_MainWnd, L"Failed Creating the Device and SwapChain", nullptr, 0);

	//Create the render target view, bind the back buffer of our swap chain as a render target
	ID3D11Texture2D* pBackBuffer;
	//In a debug build, __uuidof always initializes an object dynamically (at runtime).
	//In a release build, __uuidof can statically (at compile time) initialize an object.
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) //LPVOID data types are defined as being a "pointer to a void object". (void**)
		MessageBox(window.m_MainWnd, L"Error Getting the back buffer", nullptr, 0);
	if (FAILED(m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView)))
		MessageBox(window.m_MainWnd, L"Error Creating the Render Target View", nullptr, 0);
	pBackBuffer->Release();

	// Creates the Z Buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = Graphics::SCREENWIDTH;
	tex2dDesc.Height = Graphics::SCREENHEIGHT;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D*			ZBufferTexture;

	if (FAILED(m_Device->CreateTexture2D(&tex2dDesc, nullptr, &ZBufferTexture)))
		MessageBox(nullptr, L"Error Creating the ZBuffer Texture", nullptr, 0);

	// Creates the Z Buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_Device->CreateDepthStencilView(ZBufferTexture, &dsvDesc, &m_DepthStencilView);
	ZBufferTexture->Release();

	//Renders the TargetView and the DepthStencilView
	m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = Graphics::SCREENWIDTH;
	vp.Height = Graphics::SCREENHEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_ImmediateContext->RSSetViewports(1, &vp);

	//Implement back-face culling
	CD3D11_RASTERIZER_DESC rasterDesc(
		D3D11_FILL_SOLID,
		D3D11_CULL_NONE,
		FALSE,
		D3D11_DEFAULT_DEPTH_BIAS,
		D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		TRUE,
		FALSE,
		TRUE,
		FALSE);

	if (FAILED(m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState)))
		MessageBox(wnd.m_MainWnd, L"Error Creation the rasterizer state", nullptr, 0);

	m_ImmediateContext->RSSetState(m_RasterState);

//	//Check tutorial 03 for implementing the vertex buffer and shaders to render.
//
//	//Set up and create vertex buffer //Tutorial 03
//#include "../InitializeGraphics/createAndSetVertexBuffer.fi";
//
////Create and set up the constant buffer // Tutorial 04
//#include "../InitializeGraphics/createAndSetConstantBuffer.fi";
//
	//world = XMMatrixTranslation(0, 0, 15);
	////world *= XMMatrixRotationZ(XMConvertToRadians(RotationZ));
	//world *= XMMatrixRotationY(XMConvertToRadians(15));
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), Graphics::SCREENWIDTH / Graphics::SCREENHEIGHT, 1.0f, 100.0f);
	//view = XMMatrixIdentity();
	view = m_Camera->GetViewMatrix();
//
//	cb0_values.WorldViewProjection = world * view * projection;
//
//	cb0_values.scale = 1.0f;
//
//	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer0, 0, nullptr, &cb0_values, 0, 0);

	m_SkyBox = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_SkyBox->LoadSkyBox((char*)"Assets/cube.obj",(char*)"Assets/abovetheclouds.dds");
	m_SkyBox->SetScale(3.0f);
	
	m_ModelReflect = new ReflectModelLoader(m_Device, m_ImmediateContext, 10.0f, 0.0f, 0.0f);
	m_ModelReflect->LoadObjModel((char*)"Assets/Sphere.obj");

	m_Model = new ModelLoader(m_Device, m_ImmediateContext, 0.0f,0.0f,0.0f);
	m_Model->LoadObjModel((char*)"Assets/Sphere.obj");
	m_Model->AddTexture((char*)"Assets/uv-mapping-grid.png");
	ModelList.push_back(m_Model);

	m_Model01 = new ModelLoader(m_Device, m_ImmediateContext, 4.91f, 0.0f, 5.0f);
	m_Model01->LoadObjModel((char*)"Assets/Sphere.obj");
	m_Model01->SetScale(0.7f);
	ModelList.push_back(m_Model01);
	
	m_Model02 = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, 0.0f, 5.0f);
	m_Model02->LoadObjModel((char*)"Assets/Sphere.obj");
	m_Model02->SetScale(0.7f);
	ModelList.push_back(m_Model02);
	
	m_Gun = new ModelLoader(m_Device, m_ImmediateContext, -5.0f, 0.0f, 5.0f);
	m_Gun->LoadObjModel((char*)"Assets/USP45Game.obj");

	m_Gun->SetScale(0.2f);
	ModelList.push_back(m_Gun);


	m_Floor = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, -4.0f, 0.0f);
	m_Floor->LoadObjModel((char*)"Assets/cube.obj");
	m_Floor->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Floor->SetScale(0.3f);
	m_Floor->SetXZScale(15.0f, 15.0f);
	ModelList.push_back(m_Floor);


	//m_Floor->SetFullScale(XMFLOAT3(30.0f,1.0f,30.0f));




	//particle = new ParticleGenerator(m_Device, m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	//particle->CreateParticle();



	////Copy the vertices into the buffer
	//D3D11_MAPPED_SUBRESOURCE ms;

	////Lock the buffer to allow writting
	//m_ImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

	////Copy the data
	//memcpy(ms.pData, vertices, sizeof(vertices));

	////Unlock the buffer
	//m_ImmediateContext->Unmap(m_VertexBuffer, 0);

	////Load and compile the pixel and vertex shaders
	//ID3DBlob *VS, *PS, *error;

	//if (FAILED(D3DX11CompileFromFile(L"Shaders/VertexShader.hlsl", nullptr, nullptr, "VShader", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
	//	MessageBox(window.m_MainWnd, L"Error Compiling the Vertex Shader", nullptr, 0);

	//if (error != nullptr)//Check for shader compilation errors
	//{
	//	OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
	//	error->Release();
	//}

	////Create vertexShader object
	//if (FAILED(m_Device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &m_VertexShader)))
	//	MessageBox(window.m_MainWnd, L"Error Creating Vertex Shader", nullptr, 0);

	////Compiling the PixelShader
	//if (FAILED(D3DX11CompileFromFile(L"Shaders/PixelShader.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
	//	MessageBox(window.m_MainWnd, L"Error Compiling the Pixel Shader", nullptr, 0);

	//if (error != nullptr)
	//{
	//	OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
	//	error->Release();
	//}

	////Create PixelShader Object
	//if (FAILED(m_Device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &m_PixelShader)))
	//	MessageBox(window.m_MainWnd, L"Error Creating the Pixel Shader", nullptr, 0);

	//m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
	//m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);

	////////////////////////////////////////////////////////////////////////////
	//Tutorial 08
	//Create The Texture Mapping
	D3DX11CreateShaderResourceViewFromFile(
		m_Device,
		L"Assets/uv-mapping-grid.png",
		nullptr,
		nullptr,
		&m_TextureMap0,
		nullptr);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_Device->CreateSamplerState(&sampler_desc, &m_Sampler0);
	m_ImmediateContext->PSSetSamplers(0, 1, &m_Sampler0);
	m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureMap0);
	////////////////////////////////////////////////////////////////////////////////

	//Create Input Layout
	/*D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};*/

	/*if (FAILED(m_Device->CreateInputLayout(InputLayoutDesc, ARRAYSIZE(InputLayoutDesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout)))
		MessageBox(window.m_MainWnd, L"Error Creating the Input Layout", nullptr, 0);


	m_ImmediateContext->IASetInputLayout(m_InputLayout);*/
	//Tutorial 09
	
	m_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	m_directional_light_color = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Green
	m_ambient_light_color = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f); // Dark Grey - always use a small value for ambient lighting

	wnd.input.Initialize(wnd.m_hInst, wnd.m_MainWnd, Graphics::SCREENWIDTH, Graphics::SCREENHEIGHT);
	
}

Graphics::~Graphics()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_InputLayout) m_InputLayout->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_TextureMap0) m_TextureMap0->Release();
	if (m_Sampler0) m_Sampler0->Release();
	if (m_Device) m_Device->Release();
	delete m_Camera;
}

void Graphics::Input(GameTimer timer)
{
	//Todo: Create the game class and start using the keyboard in the game class
	if (wnd.input.IsEscapePressed())
		PostQuitMessage(0); // Exit the application
	
	if (wnd.input.KeyIsPressed(DIK_UP))
		m_Camera->Forward(2.0f * timer.DeltaTime());
	
	if (wnd.input.KeyIsPressed(DIK_DOWN))
		m_Camera->Forward(-2.0f * timer.DeltaTime());
	
	if (wnd.input.KeyIsPressed(DIK_W))
		m_Camera->Forward(5.0f * timer.DeltaTime());
		//m_Model->MoveForward(5.0f * timer.DeltaTime());
	
	if (wnd.input.KeyIsPressed(DIK_S))
		m_Camera->Forward(-5.0f * timer.DeltaTime());
		//m_Model->MoveForward(-5.0f * timer.DeltaTime());
	
	if (wnd.input.KeyIsPressed(DIK_D))
		m_Camera->Strafe(-5.0f * timer.DeltaTime());
		//m_Model->IncXPos(5.0f * timer.DeltaTime());
	
	if (wnd.input.KeyIsPressed(DIK_A))
		m_Camera->Strafe(5.0f * timer.DeltaTime());
		//m_Model->IncXPos(-5.0f * timer.DeltaTime());
	
	
}

void Graphics::Render()
{
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	view = m_Camera->GetViewMatrix();

	wnd.input.Frame();
	m_Camera->Rotate(wnd.input.m_mouseState.lX * 0.05f);
	m_Camera->Pitch(-wnd.input.m_mouseState.lY * 0.05f);

	
	//m_Camera->Rotate( * 0.00001f);
	
	//m_Camera->Pitch(wnd.mouse.GetPosY());
	

	m_SkyBox->SetPosition(m_Camera->GetPosition());
	m_SkyBox->Draw(&view, &projection);

	XMFLOAT3 cameraPos = m_Camera->GetPosition();

	//particle->DrawParticle(&view, &projection, &cameraPos);

	m_ModelReflect->Draw(&view, &projection);

	
	m_Model->LookAt_XZ(m_Camera->GetX(), m_Camera->GetZ());
	m_Model->SetPosition(m_Camera->GetPosition());
	m_Model->Draw(&view,&projection);
	//m_Model->MoveForward(0.001f);
	m_Model->TransposeLight();
	//

	for (ModelLoader* element : ModelList)
	{
		if(m_Model != element)
		{
			element->Draw(&view, &projection);
			element->TransposeLight();

			if (m_Model->CheckCollision(element))
			{
				if (m_Model->GetZPos() > element->GetZPos() && (m_Model->GetXPos() > element->GetXPos() || m_Model->GetXPos() < element->GetXPos() || m_Model->GetXPos() == element->GetXPos()))
				{
					m_Model->IncZPos(0.1f);
					m_Camera->IncZPos(0.1f);
				}
				else if (m_Model->GetXPos() > element->GetXPos() || m_Model->GetXPos() < element->GetXPos() || m_Model->GetXPos() == element->GetXPos() && m_Model->GetZPos() < element->GetZPos())
				{
					m_Model->IncZPos(-0.1f);
					m_Camera->IncZPos(-0.1f);
				}
				else if (m_Model->GetXPos() < element->GetXPos())
				{
					m_Model->IncXPos(-0.1f);
					m_Model->IncZPos(0.1f);
					m_Camera->IncXPos(-0.1f);
					m_Camera->IncZPos(0.1f);
				}
				else
				{
					m_Model->IncXPos(0.1f);
					m_Model->IncZPos(0.1f);
					m_Camera->IncXPos(0.1f);
					m_Camera->IncZPos(0.1f);
				}
			}
		}
	}

	
	/*m_Model01->IncYRotation(cos(XMConvertToRadians(rotation)));
	m_Model01->IncXRotation(sin(XMConvertToRadians(rotation)));
	m_Model01->IncZRotation(cos(XMConvertToRadians(rotation)));*/

	//m_Model->TransposeLight();
	/*UINT stride = sizeof(POS_COLOR_TEXT_NORM_VERTEX);
	UINT offset = 0;*/

	/*m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer0);
	m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ImmediateContext->PSSetSamplers(0, 1, &m_Sampler0);
	m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureMap0);*/

	//for (size_t i = 0; i < 10; i++)
	//{
	//	world = XMMatrixTranslation(0, 0, 10);
	//	//Important first the rotation and after the translation
	//	world *= XMMatrixRotationZ(RotationZ);
	//	world *= XMMatrixTranslation(4 * i, 1, 10);
	//	//view = m_Camera->GetViewMatrix();
	//	cb0_values.WorldViewProjection = world * view * projection;
	//	TransposeLight();
	//	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer0, 0, nullptr, &cb0_values, 0, 0);
	//	m_ImmediateContext->Draw(36, 0);

	//	world = XMMatrixTranslation(0, 5, 10);
	//	//Important first the rotation and after the translation
	//	world *= XMMatrixRotationZ(RotationZ);
	//	world *= XMMatrixTranslation(4, 5 * i, 10);
	//	view = m_Camera->GetViewMatrix();
	//	cb0_values.WorldViewProjection = world * view * projection;
	//	TransposeLight();
	//	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer0, 0, nullptr, &cb0_values, 0, 0);
	//	m_ImmediateContext->Draw(36, 0);
	//}
	m_SwapChain->Present(0, 0);
}

void Graphics::TransposeLight()
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Tutorial 09 Lighting
	transpose = XMMatrixTranspose(world);

	cb0_values.directional_light_color = m_directional_light_color;
	cb0_values.ambient_light_color = m_ambient_light_color;
	cb0_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, transpose);
	cb0_values.directional_light_vector = XMVector3Normalize(cb0_values.directional_light_vector);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}