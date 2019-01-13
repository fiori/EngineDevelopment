#include "../Headers/Graphics.h"
#include "../Headers/window.h"
#include <sstream>

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
		MessageBox(wnd.m_MainWnd, L"Error Getting the back buffer", nullptr, 0);
	if (FAILED(m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView)))
		MessageBox(wnd.m_MainWnd, L"Error Creating the Render Target View", nullptr, 0);
	pBackBuffer->Release();

	/*D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = FALSE;

	if(FAILED(m_Device->CreateBlendState(&b, &g_pAlphaBlendEnable)))
		MessageBox(nullptr,L"Alphablend failed", nullptr, 0);*/

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


}

void Graphics::GameLoading()
{
	wnd.input.Initialize(wnd.m_hInst, wnd.m_MainWnd, Graphics::SCREENWIDTH, Graphics::SCREENHEIGHT);
	world = XMMatrixTranslation(0, 0, 15);

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), Graphics::SCREENWIDTH / Graphics::SCREENHEIGHT, 1.0f, 100.0f);
	view = m_Camera->GetViewMatrix();
	identity = XMMatrixIdentity();

	m_particle_generator_ = new ParticleGenerator(m_Device, m_ImmediateContext, 0.0f, 4.0f, 0.0f);
	m_particle_generator_->setActive(true);
	m_particle_generator_->CreateParticle();

	//userInterface = new UILoader("Assets/font1.bmp",m_Device, m_ImmediateContext);
	//userInterface->AddText("Test", -1.0f, 1.0f, 1.0f);

	m_Camera->Pitch(-12.0f);
	m_SkyBox = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_SkyBox->LoadSkyBox((char*)"Assets/cube.obj", (char*)"Assets/abovetheclouds.dds");
	m_SkyBox->SetScale(3.0f);

	m_ModelReflect = new ReflectModelLoader(m_Device, m_ImmediateContext, 10.0f, 0.0f, 0.0f);
	m_ModelReflect->LoadObjModel((char*)"Assets/Sphere.obj");

	m_PlayerModel = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_PlayerModel->LoadObjModel((char*)"Assets/Sphere.obj");
	m_PlayerModel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	ModelList.push_back(m_PlayerModel);

	m_Floor = new ModelLoader(m_Device, m_ImmediateContext, 0.0f, -4.0f, 0.0f);
	m_Floor->LoadObjModel((char*)"Assets/cube.obj");
	m_Floor->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Floor->SetScale(0.3f);
	m_Floor->SetXZScale(15.0f, 15.0f);
	ModelList.push_back(m_Floor);

	m_Barrel = new ModelLoader(m_Device, m_ImmediateContext, -4.0f, -2.0f, 0.0f);
	m_Barrel->LoadObjModel((char*)"Assets/barrel.obj");
	m_Barrel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Barrel->SetScale(0.3f);
	ModelList.push_back(m_Barrel);

	m_RandomEnemy = new ModelLoader(m_Device, m_ImmediateContext, -2.0f, 5.0f, 0.0f);
	m_RandomEnemy->LoadObjModel((char*)"Assets/spider.obj");
	m_RandomEnemy->AddTexture((char*)"Assets/Spinnen_Bein_tex_COLOR_.png");
	m_RandomEnemy->SetScale(0.012f);
	ModelList.push_back(m_RandomEnemy);

	//m_Gun = new Weapon(m_Device, m_ImmediateContext, XMFLOAT3(0.0f,-4.0f,0.0f), -5.0f, -1.0f, 5.0f);
	//m_Gun->LoadWeapon((char*)"Assets/gun2.obj");
	//m_Gun->SetScale(XMFLOAT3(1.0f,1.0f,1.0f));
//////////////////////////////////SCENE MANAGMENT///////////////////////////////////////////////////////////////////////////////
	m_Model01 = new ModelLoader(m_Device, m_ImmediateContext, 4.91f, 0.0f, 5.0f);
	m_Model01->LoadObjModel((char*)"Assets/Sphere.obj");
	m_Model01->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Model01->SetScale(0.7f);
	ModelList.push_back(m_Model01);


	m_GunModel = new ModelLoader(m_Device, m_ImmediateContext, 1.0f, -1.0f, 3.0f);
	m_GunModel->LoadObjModel((char*)"Assets/gun2.obj");
	m_GunModel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_GunModel->SetScale(1.0f);

	m_root_node = new scene_node();
	m_PlayerNode = new scene_node();
	m_gunNode = new scene_node();

	m_PlayerNode->setModel(m_PlayerModel);
	m_gunNode->setModel(m_GunModel);
	m_root_node->addChildNode(m_PlayerNode);
	m_PlayerNode->addChildNode(m_gunNode);

	//particle = new ParticleGenerator(m_Device, m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	//particle->CreateParticle();

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
void Graphics::Input()
{
	wnd.m_Timer.Tick();
	wnd.CalculateFrameStats();

	if (wnd.input.IsEscapePressed())
		PostQuitMessage(0); // Exit the application

	if (wnd.input.KeyIsPressed(DIK_W))
		m_Camera->Forward(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());

	if (wnd.input.KeyIsPressed(DIK_S))
		m_Camera->Forward(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());

	if (wnd.input.KeyIsPressed(DIK_D))
		m_Camera->Strafe(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());

	if (wnd.input.KeyIsPressed(DIK_A))
		m_Camera->Strafe(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());



	

}

void Graphics::UpdateModel()
{
	wnd.input.Frame();
	view = m_Camera->GetViewMatrix();
	m_Camera->Rotate(wnd.input.m_mouseState.lX * SENSITIVITY);
	m_Camera->Pitch(-wnd.input.m_mouseState.lY * SENSITIVITY);


	m_SkyBox->SetPosition(m_Camera->GetPosition());
	m_PlayerModel->SetPosition(m_Camera->GetPosition());

	m_PlayerNode->SetPosition(XMFLOAT3(m_Camera->GetX(), m_Camera->GetY(), m_Camera->GetZ() - PLAYER_CAMERA_OFFSET));
	m_PlayerNode->SetYRotation(m_Camera->GetRotation());
	m_PlayerNode->SetXRotation(m_Camera->GetPitch());

	if (m_RandomEnemy->GetYPos() > GROUND_POSITION)
		m_RandomEnemy->IncYPos(GRAVITY_FORCE * wnd.m_Timer.DeltaTime());
	if (m_Barrel->GetYPos() > GROUND_POSITION)
		m_Barrel->IncYPos(GRAVITY_FORCE * wnd.m_Timer.DeltaTime());
	else if (m_Barrel->GetYPos() < GROUND_POSITION)
		m_Barrel->SetYPos(GROUND_POSITION);

	m_particle_generator_->SetPosition(m_Camera->GetPosition());
	m_RandomEnemy->Chase(m_PlayerModel, wnd.m_Timer.DeltaTime());
}

void Graphics::Render()
{
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	

	m_SkyBox->Draw(&view, &projection);
	m_ModelReflect->Draw(&view, &projection);

	m_particle_generator_->Draw(&view, &projection, &m_Camera->GetPosition(),wnd.m_Timer.DeltaTime());
	m_root_node->execute(&world, &view, &projection);

	//m_ImmediateContext->OMSetBlendState(g_pAlphaBlendEnable, 0, 0xffffffff);
	//userInterface->RenderUI();
	//m_ImmediateContext->OMSetBlendState(g_pAlphaBlendDisable, 0, 0xffffffff);

	//m_Gun->SetPosition(XMFLOAT3(m_Camera->GetPosition().x + 0.3f, m_Camera->GetPosition().y -1.0f, m_Camera->GetPosition().z + 2.0f));
	//m_Gun->Draw(&view, &projection);
	//m_Gun->SetRotation(m_Camera);



	for (ModelLoader* element : ModelList)
	{
		if (m_PlayerModel != element)
		{
			element->Draw(&view, &projection);

			if (m_PlayerModel->CheckCollision(element))
			{
				if (m_PlayerModel->GetZPos() > element->GetZPos() && (m_PlayerModel->GetXPos() > element->GetXPos() || m_PlayerModel->GetXPos() < element->GetXPos() || m_PlayerModel->GetXPos() == element->GetXPos()))
				{
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
				else if (m_PlayerModel->GetXPos() > element->GetXPos() || m_PlayerModel->GetXPos() < element->GetXPos() || m_PlayerModel->GetXPos() == element->GetXPos() && m_PlayerModel->GetZPos() < element->GetZPos())
				{
					m_PlayerModel->IncZPos(-COLLISION_OFFSET);
					m_Camera->IncZPos(-COLLISION_OFFSET);
				}
				else if (m_PlayerModel->GetXPos() < element->GetXPos())
				{
					m_PlayerModel->IncXPos(-COLLISION_OFFSET);
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncXPos(-COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
				else
				{
					m_PlayerModel->IncXPos(COLLISION_OFFSET);
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncXPos(COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
			}
		}
	}
	m_SwapChain->Present(0, 0);
}
