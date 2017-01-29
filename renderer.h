#pragma once
#include "pch.h"
#include "Shape.h"

class Renderer
{
public:
	Renderer();

	void Initialize(HWND window);
	
	void Render();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	void InitSwapChain(HWND window);
	void InitRenderTargetView(IDXGISwapChain* swapChain);
	void InitDepthStencilBuffer();
	void InitDepthStencil();
	void InitRasterizerState();
	void InitViewPort();

	void SetupTriangle();
	void SetupCube();
private:

	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_DeviceContext;

	ComPtr<IDXGISwapChain> m_SwapChain;
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	ComPtr<ID3D11RasterizerState> m_RasterizerState;
	ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
	HWND m_Window;

	Shape m_Triangle;
	Shape m_Cube;
};

