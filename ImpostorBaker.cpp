#include "ImpostorBaker.h"
#include "renderer.h"
#include "Vector2.h"

const uint32_t ImpostorBaker::ms_atlasFramesCount;
const uint32_t ImpostorBaker::ms_atlasDimension;

void ImpostorBaker::Initialize(Renderer* renderer)
{
	InitAtlasRenderTargets(renderer->GetDevice());
	InitDepthStencilState(renderer->GetDevice());
}

void ImpostorBaker::InitAtlasRenderTargets(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = ms_atlasDimension;
	desc.Height = ms_atlasDimension;
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
		device->CreateDepthStencilView(m_depthAtlasTexture.Get(), &depthStencilViewDesc, m_depthAtlasDSV.GetAddressOf())
	);
}

void ImpostorBaker::InitDepthStencilState(ID3D11Device* device)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;

	THROW_IF_FAILED(
		device->CreateDepthStencilState(
			&depthStencilDesc,
			m_depthStencilState.ReleaseAndGetAddressOf()));

}

void ImpostorBaker::Bake(ID3D11DeviceContext* context)
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(m_albedoAtlasRTV.Get(), clearColor);
	context->ClearDepthStencilView(m_depthAtlasDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, m_albedoAtlasRTV.GetAddressOf(), m_depthAtlasDSV.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	float framesMinusOne = (float)ms_atlasFramesCount - 1;

	for (float y = 0; y < ms_atlasFramesCount; ++y)
	for (float x = 0; x < ms_atlasFramesCount; ++x)
	{
		Vector2<float> vec(
			x / framesMinusOne * 2.0f - 1.0f, 
			y / framesMinusOne * 2.0f - 1.0f);

		Vector3<float> ray = OctahedralCoordToVector(vec);
		ray = ray.Normalize();

		SetViewport(context, x, y);
	}
}

Vector3<float> ImpostorBaker::OctahedralCoordToVector(const Vector2<float>& vec)
{
	Vector3<float> n(vec.x, vec.y, 1.0f - std::abs(vec.x) - std::abs(vec.y));
	float t = std::clamp(-n.y, 0.0f, 1.0f);
	n.x += n.x >= 0.0f ? -t : t;
	n.z += n.z >= 0.0f ? -t : t;
	return n;
}

void ImpostorBaker::SetViewport(ID3D11DeviceContext* context, float x, float y)
{
	float viewDimension = (float)ms_atlasDimension / (float)ms_atlasFramesCount;

	D3D11_VIEWPORT desc = {};
	desc.Width = viewDimension;
	desc.Height = viewDimension;
	desc.MinDepth = 0.0f;
	desc.MaxDepth = 1.0f;
	desc.TopLeftX = x * viewDimension;
	desc.TopLeftY = y * viewDimension;

	context->RSSetViewports(1, &desc);
}