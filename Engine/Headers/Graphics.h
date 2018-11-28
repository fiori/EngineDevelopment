#pragma once
#include <d3d11.h>
#include <dxgi.h>

class Graphics
{
public:
	Graphics(class Window& window);
	~Graphics();
	void Render();

private:
	static constexpr short SCREENWIDTH = 640;
	static constexpr short SCREENHEIGHT = 480;

private:
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_ImmediateContext;
	ID3D11RenderTargetView*	m_RenderTargetView;

};
