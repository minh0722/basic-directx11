#include "pch.h"
#include "ImpostorRenderer.h"
#include "renderer.h"
#include "GraphicsComponent.h"
#include "ImpostorBaker.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

ComPtr<ID3D11VertexShader> ImpostorRenderer::m_vs;
ComPtr<ID3D11PixelShader> ImpostorRenderer::m_ps;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vertexDataBuffer;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vsConstants;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_psConstants;
ComPtr<ID3D11ShaderResourceView> ImpostorRenderer::m_vertexDataSRV;
ComPtr<ID3D11SamplerState> ImpostorRenderer::m_samplerState;

struct QuadVertexData
{
    Vector2<float> m_uv;
};

struct VSConstant
{
    XMMATRIX worldToObject;     // float4x4
    XMVECTOR cameraWorldPos;
    float framesCount;
    float radius;
};

struct PSConstant
{
    XMMATRIX worldMatrix;
    float framesCount;
    float atlasDimension;
    float cutoff;
    float borderClamp;
};

void ImpostorRenderer::Initialize(Renderer* renderer)
{
    ID3D11Device* device = renderer->GetDevice();

    // SHADERS INITIALIZATION

    ComPtr<ID3DBlob> blob;
    THROW_IF_FAILED(D3DReadFileToBlob(L"ImpostorRenderVS.cso", blob.GetAddressOf()));
    THROW_IF_FAILED(device->CreateVertexShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_vs.GetAddressOf()));

    THROW_IF_FAILED(D3DReadFileToBlob(L"ImpostorRenderPS.cso", blob.GetAddressOf()));
    THROW_IF_FAILED(device->CreatePixelShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        m_ps.GetAddressOf()));

    // RESOURCE INITIALIZATION

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = 4 * sizeof(QuadVertexData);          // 4 vertices quad
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

    QuadVertexData verticesData[] = 
    {
        {{0.0f, 0.0f}}, // 0
        {{1.0f, 0.0f}}, // 1
        {{0.0f, 1.0f}}, // 3
        {{1.0f, 1.0f}}  // 2
    };

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = verticesData;
	THROW_IF_FAILED(device->CreateBuffer(&desc, &initData, m_vertexDataBuffer.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
    srvDesc.BufferEx.NumElements = desc.ByteWidth / sizeof(float);
    THROW_IF_FAILED(device->CreateShaderResourceView(m_vertexDataBuffer.Get(), &srvDesc, m_vertexDataSRV.GetAddressOf()));

    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = Math::RoundUpToMultiple<uint32_t>(sizeof(VSConstant), 16);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_vsConstants.GetAddressOf()));

    desc.ByteWidth = Math::RoundUpToMultiple<uint32_t>(sizeof(PSConstant), 16);
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_psConstants.GetAddressOf()));

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 9;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
    memset(samplerDesc.BorderColor, 0, 4 * sizeof(float));
    samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    THROW_IF_FAILED(device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()));
}

void ImpostorRenderer::Render(Renderer* renderer, GraphicsComponent* graphicComponent)
{
    ID3D11DeviceContext* context = renderer->GetContext();

    ID3D11ShaderResourceView* reset[] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, reset);

    context->VSSetShader(m_vs.Get(), nullptr, 0);
    context->PSSetShader(m_ps.Get(), nullptr, 0);

    context->VSSetShaderResources(0, 1, m_vertexDataSRV.GetAddressOf());
    context->VSSetConstantBuffers(0, 1, graphicComponent->GetWorldViewProjBuffer().GetAddressOf());

    Vector4f worldPos = graphicComponent->GetWorldPos();
    XMMATRIX objectToWorld = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
    XMMATRIX worldToObject = DirectX::XMMatrixInverse(nullptr, objectToWorld);
    
    const Camera& camera = renderer->GetCamera();
    const wavefront::AABB& boundingBox = graphicComponent->GetBoundingBox();
    XMVECTOR position = camera.GetPosition();

    D3D11_MAPPED_SUBRESOURCE mappedRes = {};
    THROW_IF_FAILED(context->Map(m_vsConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
    VSConstant* constant = reinterpret_cast<VSConstant*>(mappedRes.pData);
    memcpy(&constant->worldToObject, &worldToObject, sizeof(XMMATRIX));
    constant->cameraWorldPos = camera.GetPosition();
    constant->framesCount = (float)ImpostorBaker::ms_atlasFramesCount;
    constant->radius = boundingBox.GetRadius();
    context->Unmap(m_vsConstants.Get(), 0);

    context->VSSetConstantBuffers(1, 1, m_vsConstants.GetAddressOf());

    static float cutoff = 0.4f;
    static float borderClamp = 2.0f;
    THROW_IF_FAILED(context->Map(m_psConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
    PSConstant* psConstant = reinterpret_cast<PSConstant*>(mappedRes.pData);
    psConstant->atlasDimension = (float)ImpostorBaker::ms_atlasDimension;
    psConstant->cutoff = cutoff;  // alpha cutoff
    psConstant->framesCount = (float)ImpostorBaker::ms_atlasFramesCount;
    psConstant->borderClamp = borderClamp;
    memcpy(&psConstant->worldMatrix, &objectToWorld, sizeof(XMMATRIX));
    context->Unmap(m_psConstants.Get(), 0);

    context->PSSetConstantBuffers(0, 1, m_psConstants.GetAddressOf());
    context->PSSetShaderResources(0, 1, graphicComponent->GetImpostorNormalDepthSRV().GetAddressOf());
    context->PSSetShaderResources(1, 1, graphicComponent->GetImpostorAlbedoSRV().GetAddressOf());

    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    context->Draw(4, 0);
}