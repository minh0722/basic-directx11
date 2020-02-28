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
ComPtr<ID3D11Buffer> ImpostorRenderer::m_indexBuffer;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vsConstants;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_psConstants;
ComPtr<ID3D11SamplerState> ImpostorRenderer::m_samplerState;
ComPtr<ID3D11InputLayout> ImpostorRenderer::m_inputLayout;

struct QuadVertexData
{
    Vector3<float> m_vertex;
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

    std::vector<QuadVertexData> verticesData =
    {
        {{-0.5f, 0.0f, -0.5f }, {0.0f, 0.0f }},  // 0
        {{ 0.5f, 0.0f, -0.5f }, {1.0f, 0.0f }},  // 1
        {{-0.5f, 0.0f,  0.5f }, {0.0f, 1.0f }},  // 2
        {{ 0.5f, 0.0f,  0.5f }, {1.0f, 1.0f }},  // 3
        {{ 0.0f, 0.0f,  0.0f }, {0.5f, 0.5f }}   // 4
    };

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = (uint32_t)verticesData.size() * sizeof(QuadVertexData);          // 4 vertices quad
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(QuadVertexData);
	desc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = verticesData.data();
	THROW_IF_FAILED(device->CreateBuffer(&desc, &initData, m_vertexDataBuffer.GetAddressOf()));

    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = Math::RoundUpToMultiple<uint32_t>(sizeof(VSConstant), 16);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_vsConstants.GetAddressOf()));

    desc.ByteWidth = Math::RoundUpToMultiple<uint32_t>(sizeof(PSConstant), 16);
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_psConstants.GetAddressOf()));

    std::vector<uint32_t> indices = { 0, 4, 1, 1, 4, 3, 3, 4, 2, 2, 4, 0 };

    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = (uint32_t)indices.size() * sizeof(uint32_t);
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(uint32_t);
    desc.Usage = D3D11_USAGE_IMMUTABLE;

    initData.pSysMem = indices.data();

    THROW_IF_FAILED(device->CreateBuffer(&desc, &initData, m_indexBuffer.GetAddressOf()));

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

    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexShaderInputLayout(2);
    vertexShaderInputLayout[0].SemanticName = "POSITION";
    vertexShaderInputLayout[0].SemanticIndex = 0;								// will use POSITION0 semantic
    vertexShaderInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// format of the input vertex
    vertexShaderInputLayout[0].InputSlot = 0;									// 0 ~ 15
    vertexShaderInputLayout[0].AlignedByteOffset = 0;
    vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each geometry)
    vertexShaderInputLayout[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

    vertexShaderInputLayout[1].SemanticName = "TEXCOORD";
    vertexShaderInputLayout[1].SemanticIndex = 0;
    vertexShaderInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    vertexShaderInputLayout[1].InputSlot = 0;
    vertexShaderInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    vertexShaderInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    vertexShaderInputLayout[1].InstanceDataStepRate = 0;

    ID3DBlob* vertexBlob;
    THROW_IF_FAILED(D3DReadFileToBlob(L"ImpostorRenderVS.cso", &vertexBlob));

    THROW_IF_FAILED(device->CreateInputLayout(
        vertexShaderInputLayout.data(),
        (UINT)vertexShaderInputLayout.size(),
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        m_inputLayout.GetAddressOf()));
}

void ImpostorRenderer::Render(Renderer* renderer, GraphicsComponent* graphicComponent)
{
    ID3D11DeviceContext* context = renderer->GetContext();

    ID3D11ShaderResourceView* reset[] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, reset);

    context->VSSetShader(m_vs.Get(), nullptr, 0);
    context->PSSetShader(m_ps.Get(), nullptr, 0);

    uint32_t stride = sizeof(QuadVertexData), offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexDataBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
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
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(12, 0, 0);
}