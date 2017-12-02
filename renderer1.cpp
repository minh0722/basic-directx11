#include "renderer1.h"

RenderTarget::RenderTarget(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
	THROW_IF_FAILED(
		swapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(backBufferTexture.GetAddressOf())));

	THROW_IF_FAILED(
		device->CreateRenderTargetView(
			backBufferTexture.Get(),
			nullptr,
			m_RTV.GetAddressOf()));
}

RenderTarget::RenderTarget(Microsoft::WRL::ComPtr<ID3D11Device>& device, D3D11_RENDER_TARGET_VIEW_DESC desc)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> renderTarget;
	
	switch (desc.ViewDimension)
	{
	case D3D11_RTV_DIMENSION_BUFFER:
	case D3D11_RTV_DIMENSION_TEXTURE1D:
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE2D:
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE3D:
	default:
		THROW_IF_FAILED(E_FAIL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GfxDevice::Initialize(HWND window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	const D3D_FEATURE_LEVEL feature[] = { D3D_FEATURE_LEVEL_11_1 };

	THROW_IF_FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			feature,
			ARRAYSIZE(feature),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_SwapChain.GetAddressOf(),
			m_Device.GetAddressOf(),
			nullptr,
			m_DeviceContext.GetAddressOf()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer1::Initialize(HWND window)
{
	m_GfxDevice.Initialize(window);
}
