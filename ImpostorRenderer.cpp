#include "pch.h"
#include "ImpostorRenderer.h"
#include "renderer.h"
#include "GraphicsComponent.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

ComPtr<ID3D11VertexShader> ImpostorRenderer::m_vs;
ComPtr<ID3D11PixelShader> ImpostorRenderer::m_ps;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vertexDataBuffer;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vsConstants;
ComPtr<ID3D11ShaderResourceView> ImpostorRenderer::m_vertexDataSRV;

struct QuadVertexData
{
    Vector2<float> m_uv;
};

struct VSConstant
{
    XMMATRIX worldToObject;     // float4x4
    XMVECTOR cameraWorldPos;
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
	desc.StructureByteStride = sizeof(Vector2<float>);
	desc.Usage = D3D11_USAGE_IMMUTABLE;

    QuadVertexData verticesData[] = 
    {
        {{0.0f, 0.0f}},
        {{1.0f, 0.0f}},
        {{1.0f, 1.0f}},
        {{0.0f, 1.0f}}
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
}

void ImpostorRenderer::Render(Renderer* renderer, GraphicsComponent* graphicComponent)
{
    ID3D11DeviceContext* context = renderer->GetContext();

    context->VSSetShader(m_vs.Get(), nullptr, 0);
    context->PSSetShader(m_ps.Get(), nullptr, 0);

    context->VSSetShaderResources(0, 1, m_vertexDataSRV.GetAddressOf());
    context->VSSetConstantBuffers(0, 1, graphicComponent->GetWorldViewProjBuffer().GetAddressOf());

    Vector4f worldPos = graphicComponent->GetWorldPos();
    XMMATRIX worldToObject = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z));
    
    const Camera& camera = renderer->GetCamera();

    D3D11_MAPPED_SUBRESOURCE mappedRes = {};
    THROW_IF_FAILED(context->Map(m_vsConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
    VSConstant* constant = reinterpret_cast<VSConstant*>(mappedRes.pData);
    memcpy(&constant->worldToObject, &worldToObject, sizeof(XMMATRIX));
    constant->cameraWorldPos = camera.GetPosition();
    context->Unmap(m_vsConstants.Get(), 0);

    context->VSSetConstantBuffers(1, 1, m_vsConstants.GetAddressOf());
}