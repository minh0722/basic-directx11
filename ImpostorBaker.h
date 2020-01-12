#pragma once

#include "Vector3.h"
#include "Vector4f.h"

class Renderer;
class GraphicsComponent;
struct Batch;

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
	static void PrepareBake(ID3D11DeviceContext* context);
	static void Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch);
	static void Initialize(Renderer* renderer);

    static void DoProcessing(ID3D11DeviceContext* context);

	static const uint32_t ms_atlasFramesCount = 10;
	static const uint32_t ms_atlasDimension = 4096;

private:
	static void InitAtlasRenderTargets(ID3D11Device* device);
	static void InitDepthStencilState(ID3D11Device* device);
	static void InitRasterizerState(ID3D11Device* device);
	static void InitShaders(ID3D11Device* device);
	static void InitViewProjBuffer(ID3D11Device* device);
    static void InitComputeStuff(ID3D11Device* device);

	static void SetShaders(ID3D11DeviceContext* context);
	static void SetViewport(ID3D11DeviceContext* context, float x, float y);
	static void SetViewProjMatrixBuffer(ID3D11DeviceContext* context);
	static void SetRenderTargets(ID3D11DeviceContext* context);
	static void SetDepthStencilState(ID3D11DeviceContext* context);
	static void SetRasterizerState(ID3D11DeviceContext* context);

	static void UpdateViewProjMatrix(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat);

	static Vector3<float> OctahedralCoordToVector(const Vector2<float>& vec);
    static void CalculateWorkSize(uint32_t workSize, uint32_t& x, uint32_t& y, uint32_t& z);

private:
	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasRTV;
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTexture;

	static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthAtlasTexture;
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthAtlasDSV;

	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjBuffer;

    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_maskingCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_dilateCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_distanceAlphaCS;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tempAtlasTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_dilatedTexture;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tempAtlasSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dilatedTextureSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoAtlasSRV;
	static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_minDistanceBufferUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_tempAtlasUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_dilatedTextureUAV;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_minDistanceBuffer;
};