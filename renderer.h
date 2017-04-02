#pragma once
#include "pch.h"
#include "Shape.h"

#include "Vector4f.h"

class InputClass;

struct Vertex
{
	Vector4f pos;
	Vector4f color;

	Vertex(const Vector4f& pos, const Vector4f& color)
		: pos(pos), color(color) {}

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layouts(2);
		layouts[0].SemanticName = "POSITION";
		layouts[0].SemanticIndex = 0;								// will use POSITION0 semantic
		layouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
		layouts[0].InputSlot = 0;									// 0 ~ 15
		layouts[0].AlignedByteOffset = 0;
		layouts[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
		layouts[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

		layouts[1].SemanticName = "COLOR";
		layouts[1].SemanticIndex = 0;
		layouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layouts[1].InputSlot = 0;
		layouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layouts[1].InstanceDataStepRate = 0;

		return layouts;
	}
};

class Renderer
{
public:
	Renderer();

	void Initialize(HWND window);
	
	void Render(InputClass* input);

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	void InitDeviceSwapChainAndDeviceContext(HWND window);
	void InitRenderTargetView(IDXGISwapChain* swapChain);
	void InitDepthStencilBuffer();
	void InitDepthStencil();
	void InitRasterizerState();
	void InitViewPort();

	void SetupTriangle();
	void SetupCube();
    void SetupCubeForRender(InputClass* input);

    void onInput(InputClass* input, XMVECTOR& cameraPos, XMVECTOR& lookAtVector, float& fov);
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

