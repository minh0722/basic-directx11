#include "ImpostorRenderer.h"
#include "renderer.h"
#include "GraphicsComponent.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11VertexShader> ImpostorRenderer::m_vs;
ComPtr<ID3D11PixelShader> ImpostorRenderer::m_ps;
ComPtr<ID3D11Buffer> ImpostorRenderer::m_vertexConstBuffer;

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

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 4 * sizeof(Vector4f);
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_vertexConstBuffer.GetAddressOf()));
}

void ImpostorRenderer::Render(GraphicsComponent* graphicComponent)
{

}