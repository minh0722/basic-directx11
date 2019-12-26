#pragma once

#include "Vector4f.h"

class Renderer;

struct Snapshot
{
	Vector4f m_position;
	Vector4f m_ray;
};

class ImpostorBaker
{
public:
	void Initialize(Renderer* renderer);

private:
	void InitAtlasRenderTargets(ID3D11Device* device);
	void InitDepthStencilState(ID3D11Device* device);

private:
	static const uint32_t ms_atlasViewCount = 10;
	static const uint32_t ms_atlasDimension = 4096;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasRTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTexture;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthAtlasTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthAtlasRTV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	D3D11_VIEWPORT viewports[ms_atlasViewCount][ms_atlasViewCount];
};