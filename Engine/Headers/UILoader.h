#pragma once
#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <vector>

// store a sting with x and y coords (-1.0 to +1.0), and size (0.0+)
struct string_2d
{
	std::string s;
	float x;
	float y;
	float size;
};

struct POS_TEX_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Texture;
};

class UILoader
{
public:
	UILoader(std::string filename, ID3D11Device* device, ID3D11DeviceContext* context);
	~UILoader();
	void RenderUI();
	void AddText(std::string s, float x, float y, float size);

private:
	POS_TEX_VERTEX vertices[10000 * 6];
	std::vector<string_2d> s2d;

	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_ImmediateContext;

	ID3D11ShaderResourceView*	pTexture;
	ID3D11SamplerState*			pSampler;

	ID3D11Buffer*			pVertexBuffer;
	ID3D11VertexShader*		pVShader;
	ID3D11PixelShader*		pPShader;
	ID3D11InputLayout*		pInputLayout;

	ID3D11DepthStencilState* pDepthEnabledStencilState;		// state to turn on Z buffer
	ID3D11DepthStencilState* pDepthDisabledStencilState;
};
