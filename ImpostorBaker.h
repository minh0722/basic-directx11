#pragma once

#include "Vector3.h"
#include "Vector4f.h"

class Renderer;
class GraphicsComponent;

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
	static void Bake(ID3D11DeviceContext* context, GraphicsComponent* graphicsComponent);
	static void Initialize(Renderer* renderer);

private:
	static void InitAtlasRenderTargets(ID3D11Device* device);
	static void InitDepthStencilState(ID3D11Device* device);

	static void SetViewport(ID3D11DeviceContext* context, float x, float y);

	static Vector3<float> OctahedralCoordToVector(const Vector2<float>& vec);

private:
	static const uint32_t ms_atlasFramesCount = 10;
	static const uint32_t ms_atlasDimension = 4096;

	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasRTV;
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTexture;

	static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthAtlasTexture;
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthAtlasDSV;

	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};