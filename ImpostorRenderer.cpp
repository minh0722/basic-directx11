#include "ImpostorRenderer.h"
#include "renderer.h"
#include "GraphicsComponent.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11VertexShader> ImpostorRenderer::m_vs;
ComPtr<ID3D11PixelShader> ImpostorRenderer::m_ps;

void ImpostorRenderer::Initialize(Renderer* renderer)
{
    ID3D11Device* device = renderer->GetDevice();

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
}

void ImpostorRenderer::Render(GraphicsComponent* graphicComponent)
{

}