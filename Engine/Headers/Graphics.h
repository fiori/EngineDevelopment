#pragma once
#include <d3d11.h>
#include <dxgi.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "window.h"

class Graphics
{
	friend class Game;
public:

	explicit Graphics(class Window& wnd);
	~Graphics();
	void BeginFrame();
	void EndFrame();
private:
	//Constexpr helps the program to run fast and use less memory
	//If possible will be computed at compile time instead of
	//run-time
	//Source https://msdn.microsoft.com/en-us/library/dn956974.aspx [VIEWED: 17/11/2018]
	//if the declaration uses constexpr or inline (since C++17)
	//specifier, the member must be declared to have complete type (static).
	//Source: https://en.cppreference.com/w/cpp/language/static [VIEWED: 19/11/2018]
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 640;
	static constexpr float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; //RGBA

private:
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_ImmediateContext;
	ID3D11RenderTargetView*	m_RenderTargetView;

private:
	ID3D11Buffer*		m_ConstantBuffer0;
	/////////////////////////////////////////
	//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 byte boundary
	struct CONSTANT_BUFFER0
	{
		XMMATRIX WorldViewProjection; // 64 bytes
		XMVECTOR directional_light_vector; // 16 bytes
		XMVECTOR directional_light_color; // 16 bytes
		XMVECTOR ambient_light_color; // 16 bytes
		float scale;	//4 bytes
		XMFLOAT3 packing_bytes;	// 3x4 bytes = 12 bytes
	};
	CONSTANT_BUFFER0 cb0_values;
	///////////////////////////////////////
private:
	////////////
	//backface culling
	ID3D11RasterizerState* m_RasterState;
	///////////////
	//Tutorial 06
	//ZBuffer
	ID3D11DepthStencilView*		m_DepthStencilView;

private:
	//Tutorial 08
	ID3D11ShaderResourceView*		m_TextureMap0;
	ID3D11SamplerState*				m_Sampler0;
private:
	Window& wnd;
};