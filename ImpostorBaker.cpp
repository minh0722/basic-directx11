#include "ImpostorBaker.h"
#include "renderer.h"

void ImpostorBaker::Initialize(Renderer* renderer)
{
	InitAtlasRenderTargets(renderer->GetDevice());
}

void ImpostorBaker::InitAtlasRenderTargets(ID3D11Device* device)
{
	uint32_t dimension = 4098;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = dimension;
	desc.Height = dimension;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc = { 1, 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	THROW_IF_FAILED(
		device->CreateTexture2D(&desc, nullptr, m_albedoAtlasTexture.GetAddressOf()));

	THROW_IF_FAILED(
		device->CreateRenderTargetView(m_albedoAtlasTexture.Get(), nullptr, m_albedoAtlasRTV.GetAddressOf())
	);

	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	THROW_IF_FAILED(
		device->CreateTexture2D(&desc, nullptr, m_depthAtlasTexture.GetAddressOf())
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	THROW_IF_FAILED(
		device->CreateDepthStencilView(m_depthAtlasTexture.Get(), &depthStencilViewDesc, m_depthAtlasRTV.GetAddressOf())
	);
}

void ImpostorBaker::InitDepthStencilState(ID3D11Device* device)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;//true;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// Stencil operations if pixel is front_facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	THROW_IF_FAILED(
		device->CreateDepthStencilState(
			&depthStencilDesc,
			m_depthStencilState.ReleaseAndGetAddressOf()));

}