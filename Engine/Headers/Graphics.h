#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <D3DX11async.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include "../Headers/Camera.h"

class Graphics
{
public:
	Graphics(class Window& window);
	~Graphics();
	void Input();
	void Render();
	void UnlockBuffer();

private:
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 480;
	static constexpr float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA

private:
	Window& wnd;

private:
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_ImmediateContext;
	ID3D11RenderTargetView*	m_RenderTargetView;

	/////////////////////////////////////////////////////
	//Tutorial 03 Using Vertex Buffer and Shader to Render
private:
	ID3D11InputLayout*		m_InputLayout;
	ID3D11Buffer*			m_VertexBuffer;
	ID3D11VertexShader*		m_VertexShader;
	ID3D11PixelShader*		m_PixelShader;

	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos; //Position
		DirectX::XMFLOAT4 Color; //Color
	};
	/////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	//Tutorial 04 Constant Buffer
private:
	ID3D11Buffer*		m_ConstantBuffer0;

	struct CONSTANT_BUFFER0
	{
		DirectX::XMMATRIX WorldViewProjection; // 64 bytes
		float scale;	//4 bytes
		DirectX::XMFLOAT3 packing_bytes;	// 3x4 bytes = 12 bytes
	};
	CONSTANT_BUFFER0 cb0_values;

	///////////////////////////////////////
private:
	////////////
	//Tutorial05
	DirectX::XMMATRIX projection, world, view;

	//backface culling
	ID3D11RasterizerState* m_RasterState;
	///////////////

	float RotationZ = 15;

private:
	///////////
	//Tutorial 06
	//ZBuffer
	ID3D11DepthStencilView*		m_DepthStencilView;

private:
	SimpleVertex vertices[36] =
	{
		// back face
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)  ,DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)  ,DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f) ,DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},

		// front face
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)	,DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f) 	,DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) ,DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)	,DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f)	,DirectX::XMFLOAT4(1.0f,1.0f,0.0f,1.0f)},

		// left face
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f)	,DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)	,DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f)	,DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)	,DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)	,DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},

		// right face
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)  ,DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)  ,DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)   ,DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f) ,DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)  ,DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},

		// bottom face
		{DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)  ,DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f) ,DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f) ,DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},

		// top face
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)   ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f) ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f) ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)  ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)  ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f) ,DirectX::XMFLOAT4(0.0f,1.0f,1.0f,1.0f)},
	};

private:
	//////
	//Tutorial07
	Camera*	m_Camera = new Camera(0.0f, 0.0f, 0.0f, 0);
};
