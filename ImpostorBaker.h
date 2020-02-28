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

struct BakeResult
{
    const wchar_t* m_albedoBakedFileName;
    const wchar_t* m_normalBakedFileName;
};

class ImpostorBaker
{
public:
	static void PrepareBake(ID3D11DeviceContext* context);
	static BakeResult Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent);
	static void Initialize(Renderer* renderer);

    static void DoProcessing(ID3D11DeviceContext* context);

	static const uint32_t ms_atlasFramesCount = 16;
	static const uint32_t ms_atlasDimension = 4096;

private:
    static void RenderFilledPixels(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch);
	static float FindFilledPixelRatio(ID3D11DeviceContext* context);
    static void Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch);

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
    static Vector2<float> Get2DIndex(uint32_t i, uint32_t res);

private:
    
    // for finding maximum filled pixels in a frame, for maximizing atlas usage
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_singleFrameTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_singleFrameStagingTexture;
    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_singleFrameTextureRTV;

    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasMultisampledRTV;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTextureMultisampled;
	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_albedoAtlasRTV;
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoAtlasTexture;

    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthAtlasTextureMultisampled;
    static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthAtlasMultisampledDSV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthAtlasMultisampledSRV;

    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalAtlasTextureMultisampled;
    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_normalAtlasMultisampledRTV;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalAtlasTexture;
    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_normalAtlasRTV;

	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjBuffer;

    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_maskingCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_dilateCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_distanceAlphaCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_maxDistanceCS;
    static Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_distanceAlphaFinalizeCS;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tempAlbedoAtlasTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tempNormalAtlasTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_dilatedAlbedoTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_bakeAlbedoResultTexture;
    static Microsoft::WRL::ComPtr<ID3D11Texture2D> m_bakedNormalResultTexture;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tempAlbedoAtlasSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tempNormalAtlasSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dilatedAlbedoTextureSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoAtlasSRV;
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalAtlasSRV;
	static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_minDistanceBufferUAV;
	static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_maxDistanceBufferUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_tempAlbedoAtlasUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_dilatedAlbedoTextureUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_bakeAlbedoResultUAV;
    static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_bakeNormalResultUAV;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_distanceAlphaConstants;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_dilateConstants;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_minDistanceBuffer;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_maxDistanceBuffer;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_minDistancesCountConstant;
};