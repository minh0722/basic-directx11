#pragma once

class RenderTarget
{
public:
	RenderTarget(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain);
	RenderTarget(Microsoft::WRL::ComPtr<ID3D11Device>& device, D3D11_RENDER_TARGET_VIEW_DESC desc);

private:
	Microsoft::WRL::ComPtr<ID3D11Resource> m_RenderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GfxDevice
{
public:
	GfxDevice(const GfxDevice&) = delete;
	GfxDevice& operator=(const GfxDevice&) = delete;

	void Initialize(HWND window);

	void CreateRenderTarget();
	
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Renderer1
{
public:
	Renderer1(const Renderer1&) = delete;
	Renderer1& operator=(const Renderer1&) = delete;

	void Initialize(HWND window);

private:
	GfxDevice m_GfxDevice;
};