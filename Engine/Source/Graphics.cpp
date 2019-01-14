#include "../Headers/Graphics.h"

struct POS_COLOR_TEXT_NORM_VERTEX
{
	XMFLOAT3 Pos; //Position
	XMFLOAT4 Color; //Color
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

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
	const UINT CreateDeviceFlags = 0u;

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

Graphics::~Graphics()
{
	if (m_TextureMap0) m_TextureMap0->Release();
	if (m_Sampler0) m_Sampler0->Release();
	if (m_Device) m_Device->Release();
}
void Graphics::BeginFrame()
{
	wnd.m_Timer.Tick();
	wnd.CalculateFrameStats();
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void Graphics::EndFrame()
{
	m_SwapChain->Present(0, 0);
}