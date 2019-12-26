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
	void InitAtlasRenderTarget(ID3D11Device* device);

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_atlasRTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_atlasTexture;
};