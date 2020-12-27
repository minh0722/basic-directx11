#include "pch.h"
#include "ImpostorBaker.h"
#include "renderer.h"
#include "Vector2.h"
#include "GraphicsComponent.h"
#include "ObjLoader.h"
#include "GPUCapturer.h"
#include <ScreenGrab.h>
#include <wincodec.h>
#include <algorithm>

using Microsoft::WRL::ComPtr;

const uint32_t ImpostorBaker::ms_atlasFramesCount;
const uint32_t ImpostorBaker::ms_atlasDimension;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_singleFrameTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_singleFrameStagingTexture;
ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_singleFrameTextureRTV;
ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_albedoAtlasMultisampledRTV;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_albedoAtlasTextureMultisampled;
ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_albedoAtlasRTV;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_albedoAtlasTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_depthAtlasTextureMultisampled;
ComPtr<ID3D11DepthStencilView> ImpostorBaker::m_depthAtlasMultisampledDSV;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_depthAtlasMultisampledSRV;
ComPtr<ID3D11DepthStencilState> ImpostorBaker::m_depthStencilState;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_normalAtlasTextureMultisampled;
ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_normalAtlasMultisampledRTV;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_normalAtlasTexture;
ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_normalAtlasRTV;
ComPtr<ID3D11RasterizerState> ImpostorBaker::m_rasterizerState;
ComPtr<ID3D11VertexShader> ImpostorBaker::m_vertexShader;
ComPtr<ID3D11PixelShader> ImpostorBaker::m_pixelShader;
ComPtr<ID3D11Buffer> ImpostorBaker::m_viewProjBuffer;
ComPtr<ID3D11ComputeShader> ImpostorBaker::m_maskingCS;
ComPtr<ID3D11ComputeShader> ImpostorBaker::m_dilateCS;
ComPtr<ID3D11ComputeShader> ImpostorBaker::m_distanceAlphaCS;
ComPtr<ID3D11ComputeShader> ImpostorBaker::m_maxDistanceCS;
ComPtr<ID3D11ComputeShader> ImpostorBaker::m_distanceAlphaFinalizeCS;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_tempAlbedoAtlasTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_tempNormalAtlasTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_dilatedAlbedoTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_bakeAlbedoResultTexture;
ComPtr<ID3D11Texture2D> ImpostorBaker::m_bakedNormalResultTexture;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_tempAlbedoAtlasSRV;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_tempNormalAtlasSRV;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_dilatedAlbedoTextureSRV;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_albedoAtlasSRV;
ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_normalAtlasSRV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_minDistanceBufferUAV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_maxDistanceBufferUAV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_tempAlbedoAtlasUAV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_dilatedAlbedoTextureUAV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_bakeAlbedoResultUAV;
ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_bakeNormalResultUAV;
ComPtr<ID3D11Buffer> ImpostorBaker::m_distanceAlphaConstants;
ComPtr<ID3D11Buffer> ImpostorBaker::m_dilateConstants;
ComPtr<ID3D11Buffer> ImpostorBaker::m_minDistanceBuffer;
ComPtr<ID3D11Buffer> ImpostorBaker::m_maxDistanceBuffer;
ComPtr<ID3D11Buffer> ImpostorBaker::m_minDistancesCountConstant;

static const wchar_t* gs_bakedAlbedoFileName = L"AlbedoImpostorAtlas.png";
static const wchar_t* gs_bakedNormalFileName = L"NormalImpostorAtlas.png";

struct MaxDistanceConst
{
    uint32_t atlasWidth;
    uint32_t frameDimension;
    uint32_t frameCount;
    uint32_t frameX;
    uint32_t frameY;
};

struct DistanceAlphaConst
{
    uint32_t frameCount;
    uint32_t frameX;
    uint32_t frameY;
};

struct DilateConst
{
    uint32_t allChannels;
    uint32_t normalsDepth;
    uint32_t frameCount;
};

void ImpostorBaker::Initialize(Renderer* renderer)
{
    ID3D11Device* device = renderer->GetDevice();
	InitAtlasRenderTargets(device);
	InitDepthStencilState(device);
	InitRasterizerState(device);
	InitShaders(device);
	InitViewProjBuffer(device);
    InitComputeStuff(device);
}

void ImpostorBaker::InitAtlasRenderTargets(ID3D11Device* device)
{
    uint32_t singleFrameDimension = ms_atlasDimension / ms_atlasFramesCount;

	D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = singleFrameDimension;
    desc.Height = singleFrameDimension;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc = { 1, 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_singleFrameTexture.GetAddressOf()));
    THROW_IF_FAILED(device->CreateRenderTargetView(m_singleFrameTexture.Get(), nullptr, m_singleFrameTextureRTV.GetAddressOf()));

    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_singleFrameStagingTexture.GetAddressOf()));

    desc.Width = ms_atlasDimension;
    desc.Height = ms_atlasDimension;
    desc.SampleDesc = { 8, 0 };
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    THROW_IF_FAILED(
        device->CreateTexture2D(&desc, nullptr, m_albedoAtlasTextureMultisampled.GetAddressOf()));

    THROW_IF_FAILED(
        device->CreateRenderTargetView(m_albedoAtlasTextureMultisampled.Get(), nullptr, m_albedoAtlasMultisampledRTV.GetAddressOf())
    );

    THROW_IF_FAILED(
        device->CreateTexture2D(&desc, nullptr, m_normalAtlasTextureMultisampled.GetAddressOf()));

    THROW_IF_FAILED(
        device->CreateRenderTargetView(m_normalAtlasTextureMultisampled.Get(), nullptr, m_normalAtlasMultisampledRTV.GetAddressOf()));

    desc.SampleDesc = { 1, 0 };
	THROW_IF_FAILED(
		device->CreateTexture2D(&desc, nullptr, m_albedoAtlasTexture.GetAddressOf()));

	THROW_IF_FAILED(
		device->CreateRenderTargetView(m_albedoAtlasTexture.Get(), nullptr, m_albedoAtlasRTV.GetAddressOf())
	);

    THROW_IF_FAILED(
        device->CreateTexture2D(&desc, nullptr, m_normalAtlasTexture.GetAddressOf()));

    THROW_IF_FAILED(
        device->CreateRenderTargetView(m_normalAtlasTexture.Get(), nullptr, m_normalAtlasRTV.GetAddressOf()));

	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc = { 8, 0 };

    THROW_IF_FAILED(
        device->CreateTexture2D(&desc, nullptr, m_depthAtlasTextureMultisampled.GetAddressOf())
    );

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

    THROW_IF_FAILED(
        device->CreateDepthStencilView(m_depthAtlasTextureMultisampled.Get(), &depthStencilViewDesc, m_depthAtlasMultisampledDSV.GetAddressOf())
    );

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    THROW_IF_FAILED(device->CreateShaderResourceView(m_depthAtlasTextureMultisampled.Get(), &srvDesc, m_depthAtlasMultisampledSRV.GetAddressOf()));
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

void ImpostorBaker::InitRasterizerState(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = true;
	desc.MultisampleEnable = true;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	THROW_IF_FAILED(device->CreateRasterizerState(&desc, m_rasterizerState.ReleaseAndGetAddressOf()));
}

void ImpostorBaker::InitShaders(ID3D11Device* device)
{
    ShaderCompiler& shaderCompiler = Renderer::GetInstance().GetShaderCompiler();
    ComPtr<ID3DBlob> compiledBlob = shaderCompiler.CompileShader(L"../../../src/shaders/impostorBakerVertexShader.hlsl", {}, "main", ShaderType::VS);

	THROW_IF_FAILED(
		device->CreateVertexShader(
			compiledBlob->GetBufferPointer(),
			compiledBlob->GetBufferSize(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()));

    compiledBlob = shaderCompiler.CompileShader(L"../../../src/shaders/impostorBakerPixelShader.hlsl", {}, "main", ShaderType::PS);

	THROW_IF_FAILED(
		device->CreatePixelShader(
			compiledBlob->GetBufferPointer(),
			compiledBlob->GetBufferSize(),
			nullptr,
			m_pixelShader.ReleaseAndGetAddressOf()));
}

void ImpostorBaker::InitViewProjBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 2 * sizeof(DirectX::XMMATRIX);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_viewProjBuffer.GetAddressOf()));
}

void ImpostorBaker::InitComputeStuff(ID3D11Device* device)
{
    ShaderCompiler& shaderCompiler = Renderer::GetInstance().GetShaderCompiler();

    // masking cs
    ComPtr<ID3DBlob> blob = shaderCompiler.CompileShader(L"../../../src/shaders/impostorMaskingCS.hlsl", {}, "main", ShaderType::CS);

    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(), 
        blob->GetBufferSize(), 
        nullptr, 
        m_maskingCS.ReleaseAndGetAddressOf()));

    blob = shaderCompiler.CompileShader(L"../../../src/shaders/dilate.hlsl", {}, "main", ShaderType::CS);
    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_dilateCS.ReleaseAndGetAddressOf()));

    blob = shaderCompiler.CompileShader(L"../../../src/shaders/distanceAlpha.hlsl", {}, "main", ShaderType::CS);
    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_distanceAlphaCS.ReleaseAndGetAddressOf()));

    blob = shaderCompiler.CompileShader(L"../../../src/shaders/maxDistance.hlsl", {}, "main", ShaderType::CS);
    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_maxDistanceCS.ReleaseAndGetAddressOf()));

    blob = shaderCompiler.CompileShader(L"../../../src/shaders/distanceAlphaFinalize.hlsl", {}, "main", ShaderType::CS);
    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_distanceAlphaFinalizeCS.ReleaseAndGetAddressOf()));

    // temp atlas texture and srv
    D3D11_TEXTURE2D_DESC desc = {};
    m_albedoAtlasTexture->GetDesc(&desc);
    desc.MipLevels = 1;
    desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_tempAlbedoAtlasTexture.GetAddressOf()));
    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_tempNormalAtlasTexture.GetAddressOf()));
    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_dilatedAlbedoTexture.GetAddressOf()));
    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_bakeAlbedoResultTexture.GetAddressOf()));
    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_bakedNormalResultTexture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    
    THROW_IF_FAILED(device->CreateShaderResourceView(m_tempAlbedoAtlasTexture.Get(), &srvDesc, m_tempAlbedoAtlasSRV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateShaderResourceView(m_tempNormalAtlasTexture.Get(), &srvDesc, m_tempNormalAtlasSRV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateShaderResourceView(m_dilatedAlbedoTexture.Get(), &srvDesc, m_dilatedAlbedoTextureSRV.GetAddressOf()));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_tempAlbedoAtlasTexture.Get(), &uavDesc, m_tempAlbedoAtlasUAV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_dilatedAlbedoTexture.Get(), &uavDesc, m_dilatedAlbedoTextureUAV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_bakeAlbedoResultTexture.Get(), &uavDesc, m_bakeAlbedoResultUAV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_bakedNormalResultTexture.Get(), &uavDesc, m_bakeNormalResultUAV.GetAddressOf()));

    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    THROW_IF_FAILED(device->CreateShaderResourceView(m_albedoAtlasTexture.Get(), &srvDesc, m_albedoAtlasSRV.GetAddressOf()));
    THROW_IF_FAILED(device->CreateShaderResourceView(m_normalAtlasTexture.Get(), &srvDesc, m_normalAtlasSRV.GetAddressOf()));

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = ms_atlasDimension * ms_atlasDimension * sizeof(float);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	THROW_IF_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_minDistanceBuffer.GetAddressOf()));

    bufferDesc.ByteWidth = sizeof(float);
    THROW_IF_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_maxDistanceBuffer.GetAddressOf()));

	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = ms_atlasDimension * ms_atlasDimension;
	THROW_IF_FAILED(device->CreateUnorderedAccessView(m_minDistanceBuffer.Get(), &uavDesc, m_minDistanceBufferUAV.GetAddressOf()));

    uavDesc.Buffer.NumElements = 1;
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_maxDistanceBuffer.Get(), &uavDesc, m_maxDistanceBufferUAV.GetAddressOf()));

    bufferDesc.ByteWidth = 16;      // must be multiple of 16 so using sizeof(uint32_t) will fail
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.StructureByteStride = sizeof(uint32_t);

    THROW_IF_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_distanceAlphaConstants.GetAddressOf()));

    bufferDesc.ByteWidth = 32;
    bufferDesc.StructureByteStride = 0;
    THROW_IF_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_dilateConstants.GetAddressOf()));

    bufferDesc.ByteWidth = 32;
    bufferDesc.StructureByteStride = 0;
    THROW_IF_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_minDistancesCountConstant.GetAddressOf()));
}

void ImpostorBaker::PrepareBake(ID3D11DeviceContext* context)
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->ClearRenderTargetView(m_albedoAtlasMultisampledRTV.Get(), clearColor);
	context->ClearRenderTargetView(m_albedoAtlasRTV.Get(), clearColor);
    context->ClearRenderTargetView(m_normalAtlasRTV.Get(), clearColor);
    context->ClearDepthStencilView(m_depthAtlasMultisampledDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, m_albedoAtlasMultisampledRTV.GetAddressOf(), m_depthAtlasMultisampledDSV.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
	context->RSSetState(m_rasterizerState.Get());
}

BakeResult ImpostorBaker::Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent)
{
    uint32_t offset = 0;
    const auto& inputLayout = graphicsComponent->GetInputLayout();
    const auto& batches = graphicsComponent->GetBatches();
    const auto& materialBuffers = graphicsComponent->GetMaterialBuffers();

	float clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(m_singleFrameTextureRTV.Get(), clear);

    for (auto it = batches.begin(); it != batches.end(); ++it)
    {
        const uint32_t materialID = it->first;
        const Batch& batch = it->second;

        context->IASetVertexBuffers(0, 1, batch.vertexBuffer.GetAddressOf(), &batch.vertexBufferStride, &offset);
        context->IASetInputLayout(inputLayout.Get());
        context->PSSetConstantBuffers(0, 1, materialBuffers.at(materialID).GetAddressOf());
        context->IASetPrimitiveTopology(batch.m_topology);

        RenderFilledPixels(context, graphicsComponent, batch);
    }

	float ratio = FindFilledPixelRatio(context);
    graphicsComponent->SetOctRadius(graphicsComponent->GetBoundingBox().GetRadius() * ratio);

    ImpostorBaker::PrepareBake(context);
    for (auto it = batches.begin(); it != batches.end(); ++it)
    {
        const uint32_t materialID = it->first;
        const Batch& batch = it->second;

        context->IASetVertexBuffers(0, 1, batch.vertexBuffer.GetAddressOf(), &batch.vertexBufferStride, &offset);
        context->IASetInputLayout(inputLayout.Get());
        context->PSSetConstantBuffers(0, 1, materialBuffers.at(materialID).GetAddressOf());
        context->IASetPrimitiveTopology(batch.m_topology);
        Bake(context, graphicsComponent, batch);
    }

    DoProcessing(context);

    return BakeResult{ gs_bakedAlbedoFileName, gs_bakedNormalFileName };
}

void ImpostorBaker::RenderFilledPixels(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch)
{
    float framesMinusOne = (float)ms_atlasFramesCount - 1;

    const auto& boundingBox = graphicsComponent->GetBoundingBox();
    const Vector4f& center = boundingBox.m_center;
    auto lookat = DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f);
    float radius = graphicsComponent->GetOctRadius();
    float diameter = radius * 2.0f;

    /// render without clearing to check for miximum filled pixels in a frame
    context->OMSetRenderTargets(1, m_singleFrameTextureRTV.GetAddressOf(), nullptr);
    context->OMSetDepthStencilState(nullptr, 0);
    SetRasterizerState(context);
    SetShaders(context);

    float singleFrameDimension = ms_atlasDimension / ms_atlasFramesCount;
    for (float y = 0; y < ms_atlasFramesCount; ++y)
    for (float x = 0; x < ms_atlasFramesCount; ++x)
    {
        Vector2<float> vec(
            x / framesMinusOne * 2.0f - 1.0f,
            y / framesMinusOne * 2.0f - 1.0f);

        Vector3<float> ray = OctahedralCoordToVector(vec).Normalize();
        DirectX::XMVECTOR xmRay = DirectX::XMVectorSet(ray.x, ray.y, ray.z, 1.0f);

        const Vector4f position = Vector4f(boundingBox.m_center.XYZ() + ray * radius, 1.0f);

        auto xmvecPos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);
        auto globalUp = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);

        if (DirectX::XMComparisonAllTrue(DirectX::XMVector4EqualR(DirectX::XMVector3Cross(xmRay, globalUp), DirectX::XMVectorZero())))
            globalUp = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);

        DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtRH(xmvecPos, lookat, globalUp);
        DirectX::XMMATRIX projMatrix = DirectX::XMMatrixOrthographicRH(diameter, diameter, 0.0f, diameter);

        UpdateViewProjMatrix(context, viewMatrix, projMatrix);
        SetViewProjMatrixBuffer(context);

        D3D11_VIEWPORT viewport;
        viewport.Width = singleFrameDimension;
        viewport.Height = singleFrameDimension;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        context->RSSetViewports(1, &viewport);

        context->Draw(batch.verticesCount, 0);
    }
}

float ImpostorBaker::FindFilledPixelRatio(ID3D11DeviceContext* context)
{
	float singleFrameDimension = ms_atlasDimension / ms_atlasFramesCount;

	context->CopyResource(m_singleFrameStagingTexture.Get(), m_singleFrameTexture.Get());

	Vector2<float> min(singleFrameDimension - 1);
	Vector2<float> max(0);

	D3D11_MAPPED_SUBRESOURCE mappedRes = {};
	THROW_IF_FAILED(context->Map(m_singleFrameStagingTexture.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedRes));
	char* colors = reinterpret_cast<char*>(mappedRes.pData);
	uint32_t rowPitch = mappedRes.RowPitch;

	uint32_t pixelsCount = uint32_t(singleFrameDimension * singleFrameDimension);
	for (uint32_t i = 0; i < pixelsCount; ++i)
	{
		uint8_t r = *(colors + i * 4);
		uint8_t g = *(colors + i * 4 + 1);
		uint8_t b = *(colors + i * 4 + 2);
		uint8_t a = *(colors + i * 4 + 3);

		if (r != 0x00 || g != 0x00 || b != 0x00 || a != 0x00)
		{
			auto texPos = Get2DIndex(i, (uint32_t)singleFrameDimension);
			min.x = std::min(min.x, texPos.x);
			min.y = std::min(min.y, texPos.y);
			max.x = std::max(max.x, texPos.x);
			max.y = std::max(max.y, texPos.y);
		}
	}

	context->Unmap(m_singleFrameStagingTexture.Get(), 0);

	Vector2<float> len(max.x - min.x, max.y - min.y);
	float maxLen = std::max(len.x, len.y);

	float ratio = maxLen / singleFrameDimension;

	return ratio;
}

void ImpostorBaker::Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch)
{
    float framesMinusOne = (float)ms_atlasFramesCount - 1;

    const auto& boundingBox = graphicsComponent->GetBoundingBox();
    const Vector4f& center = boundingBox.m_center;
    auto lookat = DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f);
    float radius = graphicsComponent->GetOctRadius();
    float diameter = radius * 2.0f;

	SetRenderTargets(context);
	SetDepthStencilState(context);
	SetRasterizerState(context);
	SetShaders(context);

	for (float y = 0; y < ms_atlasFramesCount; ++y)
	for (float x = 0; x < ms_atlasFramesCount; ++x)
	{
		Vector2<float> vec(
			x / framesMinusOne * 2.0f - 1.0f, 
			y / framesMinusOne * 2.0f - 1.0f);

		Vector3<float> ray = OctahedralCoordToVector(vec).Normalize();
        DirectX::XMVECTOR xmRay = DirectX::XMVectorSet(ray.x, ray.y, ray.z, 1.0f);

		const Vector4f position = Vector4f(boundingBox.m_center.XYZ() + ray * radius, 1.0f);

		auto xmvecPos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);
		auto globalUp = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);

        if (DirectX::XMComparisonAllTrue(DirectX::XMVector4EqualR(DirectX::XMVector3Cross(xmRay, globalUp), DirectX::XMVectorZero())))
            globalUp = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);

		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtRH(xmvecPos, lookat, globalUp);
		DirectX::XMMATRIX projMatrix = DirectX::XMMatrixOrthographicRH(diameter, diameter, 0.0f, diameter);

		UpdateViewProjMatrix(context, viewMatrix, projMatrix);
		SetViewProjMatrixBuffer(context);

		SetViewport(context, x, y);

        // baking also merge depth with the normal
		context->Draw(batch.verticesCount, 0);
	}
}

void ImpostorBaker::CalculateWorkSize(uint32_t workSize, uint32_t& x, uint32_t& y, uint32_t& z)
{
    const uint32_t GROUP_SIZE = 256;
    const uint32_t MAX_DIM_GROUPS = 1024;
    const uint32_t MAX_DIM_THREADS = (GROUP_SIZE * MAX_DIM_GROUPS);

    if (workSize <= MAX_DIM_THREADS)
    {
        x = (workSize - 1) / GROUP_SIZE + 1;
        y = z = 1;
    }
    else
    {
        x = MAX_DIM_GROUPS;
        y = (workSize - 1) / MAX_DIM_THREADS + 1;
        z = 1;
    }
}

Vector2<float> ImpostorBaker::Get2DIndex(uint32_t i, uint32_t res)
{
    float x = float(i % res);
    float y = (i - x) / res;
    return Vector2<float>(x, y);
}

void ImpostorBaker::DoProcessing(ID3D11DeviceContext* context)
{
    context->ResolveSubresource(m_albedoAtlasTexture.Get(), 0, m_albedoAtlasTextureMultisampled.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
    context->ResolveSubresource(m_normalAtlasTexture.Get(), 0, m_normalAtlasTextureMultisampled.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    ID3D11ShaderResourceView* resetSRV[] = { nullptr, nullptr };
    ID3D11UnorderedAccessView* resetUAV[] = { nullptr };

    auto ResetStates = [&resetSRV, &resetUAV, &context]() 
    {
        // reset
        context->CSSetShader(nullptr, nullptr, 0);
        context->CSSetShaderResources(0, 2, resetSRV);
        context->CSSetUnorderedAccessViews(0, 1, resetUAV, nullptr);
    };

    // unbind the albedoatlas since it was the render target
    context->OMSetRenderTargets(0, nullptr, nullptr);

    uint32_t x, y, z;
    CalculateWorkSize(ms_atlasDimension * ms_atlasDimension, x, y, z);

    // first do masking of the albedo atlas
    context->CSSetShader(m_maskingCS.Get(), nullptr, 0);
    context->CSSetShaderResources(0, 1, m_albedoAtlasSRV.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, m_tempAlbedoAtlasUAV.GetAddressOf(), nullptr);
    context->Dispatch(x, y, z);

    ResetStates();

    auto SetDilateConstant = [&context](uint32_t _allChannels, uint32_t _normalsDepth)
    {
        D3D11_MAPPED_SUBRESOURCE mappedRes = {};
        THROW_IF_FAILED(context->Map(m_dilateConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
        DilateConst* dilateConstant = reinterpret_cast<DilateConst*>(mappedRes.pData);
        dilateConstant->allChannels = _allChannels;
        dilateConstant->normalsDepth = _normalsDepth;
        dilateConstant->frameCount = ms_atlasFramesCount;
        context->Unmap(m_dilateConstants.Get(), 0);
    };

	ResetStates();

    // then do dilating on normal
    SetDilateConstant(1, 1);
    context->CSSetShader(m_dilateCS.Get(), nullptr, 0);
    context->CSSetShaderResources(0, 1, m_normalAtlasSRV.GetAddressOf());
    context->CSSetShaderResources(1, 1, m_tempAlbedoAtlasSRV.GetAddressOf());   // use same mask as albedo
    context->CSSetConstantBuffers(0, 1, m_dilateConstants.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, m_bakeNormalResultUAV.GetAddressOf(), nullptr);
    context->Dispatch(x, y, z);

    ResetStates();

    // then do dilating on albedo
    SetDilateConstant(0, 0);
    context->CSSetShader(m_dilateCS.Get(), nullptr, 0);
    context->CSSetShaderResources(0, 1, m_albedoAtlasSRV.GetAddressOf());
    context->CSSetShaderResources(1, 1, m_tempAlbedoAtlasSRV.GetAddressOf());
    context->CSSetConstantBuffers(0, 1, m_dilateConstants.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, m_dilatedAlbedoTextureUAV.GetAddressOf(), nullptr);
    context->Dispatch(x, y, z);

    ResetStates();

    DistanceAlphaConst constant;
    uint32_t frameSize = ms_atlasDimension / ms_atlasFramesCount;
    CalculateWorkSize(frameSize * frameSize, x, y, z);

    for(uint32_t r = 0; r < ms_atlasFramesCount; ++r)
    for(uint32_t c = 0; c < ms_atlasFramesCount; ++c)
    {
        constant.frameCount = ms_atlasFramesCount;
        constant.frameX = c;
        constant.frameY = r;

        // distance alpha (min distances)
        D3D11_MAPPED_SUBRESOURCE mappedRes = {};
        THROW_IF_FAILED(context->Map(m_distanceAlphaConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
        DistanceAlphaConst* mappedConst = reinterpret_cast<DistanceAlphaConst*>(mappedRes.pData);
        memcpy(mappedConst, &constant, sizeof(DistanceAlphaConst));
        context->Unmap(m_distanceAlphaConstants.Get(), 0);
        context->CSSetShader(m_distanceAlphaCS.Get(), nullptr, 0);
        context->CSSetShaderResources(0, 1, m_tempAlbedoAtlasSRV.GetAddressOf());
        context->CSSetUnorderedAccessViews(0, 1, m_minDistanceBufferUAV.GetAddressOf(), nullptr);
        context->CSSetConstantBuffers(0, 1, m_distanceAlphaConstants.GetAddressOf());
        context->Dispatch(x, y, z);

        ResetStates();

        // max distance
        uint32_t count = (ms_atlasDimension / ms_atlasFramesCount) * (ms_atlasDimension / ms_atlasFramesCount);
        THROW_IF_FAILED(context->Map(m_minDistancesCountConstant.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
        MaxDistanceConst* maxDistConst = reinterpret_cast<MaxDistanceConst*>(mappedRes.pData);
        maxDistConst->atlasWidth = ms_atlasDimension;
        maxDistConst->frameDimension = ms_atlasDimension / ms_atlasFramesCount;
        maxDistConst->frameCount = ms_atlasFramesCount;
        maxDistConst->frameX = c;
        maxDistConst->frameY = r;
        context->Unmap(m_minDistancesCountConstant.Get(), 0);

        context->CSSetShader(m_maxDistanceCS.Get(), nullptr, 0);
        context->CSSetConstantBuffers(0, 1, m_minDistancesCountConstant.GetAddressOf());
        context->CSSetUnorderedAccessViews(0, 1, m_minDistanceBufferUAV.GetAddressOf(), nullptr);
        context->CSSetUnorderedAccessViews(1, 1, m_maxDistanceBufferUAV.GetAddressOf(), nullptr);
        context->Dispatch(1, 1, 1);

        // distance alpha finalize
        ResetStates();
        context->CSSetShader(m_distanceAlphaFinalizeCS.Get(), nullptr, 0);
        context->CSSetConstantBuffers(0, 1, m_distanceAlphaConstants.GetAddressOf());
        context->CSSetShaderResources(0, 1, m_dilatedAlbedoTextureSRV.GetAddressOf());
        context->CSSetUnorderedAccessViews(0, 1, m_minDistanceBufferUAV.GetAddressOf(), nullptr);
        context->CSSetUnorderedAccessViews(1, 1, m_maxDistanceBufferUAV.GetAddressOf(), nullptr);
        context->CSSetUnorderedAccessViews(2, 1, m_bakeAlbedoResultUAV.GetAddressOf(), nullptr);
        context->Dispatch(x, y, z);

        ResetStates();
    }
    
    ResetStates();

    THROW_IF_FAILED(DirectX::SaveWICTextureToFile(context, m_bakeAlbedoResultTexture.Get(), GUID_ContainerFormatPng, gs_bakedAlbedoFileName, &GUID_WICPixelFormat32bppBGRA));
    THROW_IF_FAILED(DirectX::SaveWICTextureToFile(context, m_bakedNormalResultTexture.Get(), GUID_ContainerFormatPng, gs_bakedNormalFileName, &GUID_WICPixelFormat32bppBGRA));
}

Vector3<float> ImpostorBaker::OctahedralCoordToVector(const Vector2<float>& vec)
{
	Vector3<float> n(vec.x, 1.0f - std::abs(vec.x) - std::abs(vec.y), vec.y);
	float t = std::clamp(-n.y, 0.0f, 1.0f);
	n.x += n.x >= 0.0f ? -t : t;
	n.z += n.z >= 0.0f ? -t : t;
	return n;
}

void ImpostorBaker::SetShaders(ID3D11DeviceContext* context)
{
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
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

void ImpostorBaker::SetViewProjMatrixBuffer(ID3D11DeviceContext* context)
{
	context->VSSetConstantBuffers(0, 1, m_viewProjBuffer.GetAddressOf());
}

void ImpostorBaker::SetRenderTargets(ID3D11DeviceContext* context)
{
    ID3D11RenderTargetView* rtvs[] = 
    {
        m_albedoAtlasMultisampledRTV.Get(),
        m_normalAtlasMultisampledRTV.Get()
    };

	context->OMSetRenderTargets(2, rtvs, m_depthAtlasMultisampledDSV.Get());
}

void ImpostorBaker::SetDepthStencilState(ID3D11DeviceContext* context)
{
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
}

void ImpostorBaker::SetRasterizerState(ID3D11DeviceContext* context)
{
	context->RSSetState(m_rasterizerState.Get());
}

void ImpostorBaker::UpdateViewProjMatrix(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	THROW_IF_FAILED(context->Map(m_viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, &viewMat, sizeof(DirectX::XMMATRIX));
	memcpy((uint8_t*)mappedResource.pData + sizeof(DirectX::XMMATRIX), &projMat, sizeof(DirectX::XMMATRIX));

	context->Unmap(m_viewProjBuffer.Get(), 0);
}