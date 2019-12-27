#pragma once

#include "Vector3.h"
#include "Vector4f.h"

class Renderer;

template <typename T>
class Vector2;

struct Snapshot
{
	Vector4f m_position;
	Vector4f m_ray;
};

class ImpostorBaker
{
public:
	void Initialize(Renderer* renderer);

	void Bake(ID3D11DeviceContext* context);

private:
	void InitAtlasRenderTargets(ID3D11Device* device);
	void InitDepthStencilState(ID3D11Device* device);

	void SetViewport(ID3D11DeviceContext* context, float x, float y);

	Vector3<float> OctahedralCoordToVector(const Vector2<float>& vec);

private:
	static const uint32_t ms_atlasFramesCount = 10;
	static const uint32_t ms_atlasDimension = 4096;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasRTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTexture;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthAtlasTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthAtlasDSV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};