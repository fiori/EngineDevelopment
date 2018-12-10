#include "../Headers/Graphics.h"
#include "../Headers/window.h"

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

#if defined(DEBUG) || defined(_DEBUG)
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
	m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

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

	//Check tutorial 03 for implementing the vertex buffer and shaders to render.

	//Set up and create vertex buffer //Tutorial 03
#include "../InitializeGraphics/createAndSetVertexBuffer.fi";

//Create and set up the constant buffer // Tutorial 04
#include "../InitializeGraphics/createAndSetConstantBuffer.fi";

	world = DirectX::XMMatrixTranslation(0, 0, 15);
	world *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(RotationZ));
	projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0), Graphics::SCREENWIDTH / Graphics::SCREENHEIGHT, 1.0f, 100.0f);
	view = DirectX::XMMatrixIdentity();

	cb0_values.WorldViewProjection = world * view * projection;

	cb0_values.scale = 1.0f;

	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer0, 0, nullptr, &cb0_values, 0, 0);

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writting
	m_ImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	m_ImmediateContext->Unmap(m_VertexBuffer, 0);

	//Load and compile the pixel and vertex shaders
	ID3DBlob *VS, *PS, *error;

	if (FAILED(D3DX11CompileFromFile(L"Shaders/VertexShader.hlsl", nullptr, nullptr, "VShader", "vs_4_0", 0, 0, nullptr, &VS, &error, nullptr)))
		MessageBox(window.m_MainWnd, L"Error Compiling the Vertex Shader", nullptr, 0);

	if (error != nullptr)//Check for shader compilation errors
	{
		OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
		error->Release();
	}

	//Create vertexShader object
	if (FAILED(m_Device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &m_VertexShader)))
		MessageBox(window.m_MainWnd, L"Error Creating Vertex Shader", nullptr, 0);

	//Compiling the PixelShader
	if (FAILED(D3DX11CompileFromFile(L"Shaders/PixelShader.hlsl", nullptr, nullptr, "PShader", "ps_4_0", 0, 0, nullptr, &PS, &error, nullptr)))
		MessageBox(window.m_MainWnd, L"Error Compiling the Pixel Shader", nullptr, 0);

	if (error != nullptr)
	{
		OutputDebugStringW(static_cast<wchar_t*>(error->GetBufferPointer()));
		error->Release();
	}

	//Create PixelShader Object
	if (FAILED(m_Device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &m_PixelShader)))
		MessageBox(window.m_MainWnd, L"Error Creating the Pixel Shader", nullptr, 0);

	m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);

	//Create Input Layout
	D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	if (FAILED(m_Device->CreateInputLayout(InputLayoutDesc, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout)))
		MessageBox(window.m_MainWnd, L"Error Creating the Input Layout", nullptr, 0);

	m_ImmediateContext->IASetInputLayout(m_InputLayout);
}

Graphics::~Graphics()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_InputLayout) m_InputLayout->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();
}

void Graphics::Render()
{
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	//Todo: Create the game class and start using the keyboard in the game class

	if (wnd.kbd.KeyIsPressed(VK_UP))
	{
		cb0_values.scale += 0.0002f;
	}
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		cb0_values.scale -= 0.0002f;
	}
	if (wnd.kbd.KeyIsPressed(0x45))
	{
		RotationZ += 1.0f;
		world *= DirectX::XMMatrixRotationZ((DirectX::XMConvertToRadians(RotationZ)));
		cb0_values.WorldViewProjection = world * view * projection;
	}

	m_ImmediateContext->UpdateSubresource(m_ConstantBuffer0, 0, nullptr, &cb0_values, 0, 0);

	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer0);

	m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ImmediateContext->Draw(36, 0);

	m_SwapChain->Present(0, 0);
}