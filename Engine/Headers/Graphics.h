#pragma once
#include <d3d11.h>
#include <dxgi.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "../Headers/Includes.h"
//#include "../Headers/UILoader.h"
#include <list>


//#include "../Headers/ParticleGenerator.h"
class Graphics
{
public:
	Graphics(class Window& window);
	~Graphics();
	void Input(GameTimer timer);
	void Render();

	//float getGravityForce(ModelLoader * objectOne, ModelLoader * objectTwo);


private:
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 640;
	static constexpr float ClearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; //RGBA

private:
	Window& wnd;
	//UILoader* userInterface;
	float mouseXPastPos;
	bool mouseMoved = false;
	int x, y;


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
	//ID3D11BlendState*		g_pAlphaBlendEnable; //14
	//ID3D11BlendState*		g_pAlphaBlendDisable; //14

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
	XMMATRIX projection, world, view, identity;

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
	ModelLoader* m_PlayerModel;
	ModelLoader* m_GunModel;
	ModelLoader* m_SkyBox;
	ModelLoader* m_RandomEnemy;
	ModelLoader* m_Floor;
	ModelLoader* m_Barrel;
	ModelLoader* m_Model01;
	ReflectModelLoader* m_ModelReflect;
	std::list<ModelLoader*> ModelList;

	//ParticleGenerator* particle;
	scene_node* m_root_node, *m_PlayerNode, *m_gunNode;
};
