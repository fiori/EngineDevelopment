#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <D3DX11async.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>

class Graphics
{
public:
	Graphics(class Window& window);
	~Graphics();
	void Render();

private:
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 480;
	static constexpr float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA


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

};
