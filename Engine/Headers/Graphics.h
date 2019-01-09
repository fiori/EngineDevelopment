#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <D3DX11async.h>
#include <D3DCompiler.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "../Headers/Camera.h"
#include "../Headers/ModelLoader.h"
#include "../Headers/ReflectModelLoader.h"
#include "../Headers/GameTimer.h"
class Graphics
{
public:
	Graphics(class Window& window);
	~Graphics();
	void Input(GameTimer timer);
	void Render();
	void TransposeLight();
	void UnlockBuffer();


private:
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 640;
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

	struct POS_COLOR_TEXT_NORM_VERTEX
	{
		XMFLOAT3 Pos; //Position
		XMFLOAT4 Color; //Color
		XMFLOAT2 Texture0;
		XMFLOAT3 Normal;
	};
	/////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	//Tutorial 04 Constant Buffer
private:
	ID3D11Buffer*		m_ConstantBuffer0;

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
	//Tutorial05
	XMMATRIX projection, world, view;

	//backface culling
	ID3D11RasterizerState* m_RasterState;
	///////////////

	float RotationZ = 0;

private:
	///////////
	//Tutorial 06
	//ZBuffer
	ID3D11DepthStencilView*		m_DepthStencilView;

private:
	POS_COLOR_TEXT_NORM_VERTEX vertices[36] =
	{
		// back face
		{XMFLOAT3(-1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(-1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(-1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		// front face
		{XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{XMFLOAT3(1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		// left face
		{XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(-1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(-1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		// right face
		{XMFLOAT3(1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f) , XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f) , XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f) , XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(-1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
																													
		// bottom face
		{XMFLOAT3(1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{XMFLOAT3(1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{XMFLOAT3(1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, -1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		// top face
		{XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, 1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{XMFLOAT3(-1.0f, 1.0f, -1.0f),	XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }
	};

private:
	//////
	//Tutorial07
	Camera*	m_Camera = new Camera(0.0f, 0.0f, -15.0f, 0, 0.0f);
	//Tutorial 08
	ID3D11ShaderResourceView*		m_TextureMap0;
	ID3D11SamplerState*				m_Sampler0;
	


	//Tutorial 09
	XMVECTOR m_directional_light_shines_from;
	
	XMVECTOR m_directional_light_color;
	
	XMVECTOR m_ambient_light_color;

	XMMATRIX transpose;

private:
	//Tutorial 10
	ModelLoader* m_Model;
	ModelLoader* m_Model01;
	ReflectModelLoader* m_ModelReflect;
	ModelLoader* m_SkyBox;
};
