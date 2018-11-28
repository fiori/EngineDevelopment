#include "../Headers/Graphics.h"
#include "../Headers/window.h"

Graphics::Graphics(Window& window)
{
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::SCREENWIDTH;
	sd.BufferDesc.Height = Graphics::SCREENHEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window.m_MainWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	if (!D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_SwapChain,
		&m_Device,
		nullptr,
		&m_ImmediateContext)
		)
		MessageBox(nullptr, L"Failed Creating the Device and SwapChain", nullptr, 0);

	//Create the render target view, bind the back buffer of our swap chain as a render target
	ID3D11Resource* pBackBuffer;
	//In a debug build, __uuidof always initializes an object dynamically (at runtime).
	//In a release build, __uuidof can statically (at compile time) initialize an object.
	if (!m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)) //LPVOID data types are defined as being a "pointer to a void object". (void**)
		MessageBox(window.m_MainWnd, L"Error Getting the back buffer", nullptr, 0);
	if (!m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView))
		MessageBox(window.m_MainWnd, L"Error Creating the Render Target View", nullptr, 0);
	m_ImmediateContext->OMSetRenderTargets(0, &m_RenderTargetView, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = Graphics::SCREENWIDTH;
	vp.Height = Graphics::SCREENHEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_ImmediateContext->RSSetViewports(1, &vp);
}

Graphics::~Graphics()
{
}

void Graphics::Render()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; //RGBA
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);
	m_SwapChain->Present(0, 0);
}