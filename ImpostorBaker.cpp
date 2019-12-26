#include "ImpostorBaker.h"
#include "renderer.h"

void ImpostorBaker::Initialize(Renderer* renderer)
{
	InitAtlasRenderTarget(renderer->GetDevice());
}

void ImpostorBaker::InitAtlasRenderTarget(ID3D11Device* device)
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
		device->CreateTexture2D(&desc, nullptr, m_atlasTexture.GetAddressOf()));

	THROW_IF_FAILED(
		device->CreateRenderTargetView(m_atlasTexture.Get(), nullptr, m_atlasRTV.GetAddressOf())
	);
}