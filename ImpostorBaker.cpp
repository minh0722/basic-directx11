#include "pch.h"
#include "ImpostorBaker.h"
#include "renderer.h"
#include "Vector2.h"
#include "GraphicsComponent.h"
#include "ObjLoader.h"
#include <ScreenGrab.h>
#include <wincodec.h>

const uint32_t ImpostorBaker::ms_atlasFramesCount;
const uint32_t ImpostorBaker::ms_atlasDimension;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ImpostorBaker::m_albedoAtlasRTV;
Microsoft::WRL::ComPtr<ID3D11Texture2D> ImpostorBaker::m_albedoAtlasTexture;
Microsoft::WRL::ComPtr<ID3D11Texture2D> ImpostorBaker::m_depthAtlasTexture;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ImpostorBaker::m_depthAtlasDSV;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> ImpostorBaker::m_depthStencilState;
Microsoft::WRL::ComPtr<ID3D11RasterizerState> ImpostorBaker::m_rasterizerState;
Microsoft::WRL::ComPtr<ID3D11VertexShader> ImpostorBaker::m_vertexShader;
Microsoft::WRL::ComPtr<ID3D11PixelShader> ImpostorBaker::m_pixelShader;
Microsoft::WRL::ComPtr<ID3D11Buffer> ImpostorBaker::m_viewProjBuffer;
Microsoft::WRL::ComPtr<ID3D11ComputeShader> ImpostorBaker::m_maskingCS;
Microsoft::WRL::ComPtr<ID3D11Texture2D> ImpostorBaker::m_tempAtlasTexture;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_tempAtlasSRV;
Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> ImpostorBaker::m_tempAtlasUAV;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_albedoAtlasSRV;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ImpostorBaker::m_depthAtlasSRV;

void ImpostorBaker::Initialize(Renderer* renderer)
{
    ID3D11Device* device = renderer->GetDevice();
	InitAtlasRenderTargets(device);
	InitDepthStencilState(device);
	InitRasterizerState(device);
	InitShaders(device);
	InitViewProjBuffer(device);
    //InitComputeStuff(device);
}

void ImpostorBaker::InitAtlasRenderTargets(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = ms_atlasDimension;
	desc.Height = ms_atlasDimension;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc = { 1, 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	THROW_IF_FAILED(
		device->CreateTexture2D(&desc, nullptr, m_albedoAtlasTexture.GetAddressOf()));

	THROW_IF_FAILED(
		device->CreateRenderTargetView(m_albedoAtlasTexture.Get(), nullptr, m_albedoAtlasRTV.GetAddressOf())
	);

	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	THROW_IF_FAILED(
		device->CreateTexture2D(&desc, nullptr, m_depthAtlasTexture.GetAddressOf())
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	THROW_IF_FAILED(
		device->CreateDepthStencilView(m_depthAtlasTexture.Get(), &depthStencilViewDesc, m_depthAtlasDSV.GetAddressOf())
	);
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
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	THROW_IF_FAILED(device->CreateRasterizerState(&desc, m_rasterizerState.ReleaseAndGetAddressOf()));
}

void ImpostorBaker::InitShaders(ID3D11Device* device)
{
	ID3DBlob* blob;
	THROW_IF_FAILED(D3DReadFileToBlob(L"impostorBakerVertexShader.cso", &blob));

	THROW_IF_FAILED(
		device->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()));

	THROW_IF_FAILED(D3DReadFileToBlob(L"SpaceshipPixelShader.cso", &blob));

	THROW_IF_FAILED(
		device->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
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
    // masking cs
    ID3DBlob* blob;
    THROW_IF_FAILED(D3DReadFileToBlob(L"impostorMaskingCS.cso", &blob));

    THROW_IF_FAILED(device->CreateComputeShader(
        blob->GetBufferPointer(), 
        blob->GetBufferSize(), 
        nullptr, 
        m_maskingCS.ReleaseAndGetAddressOf()));

    // temp atlas texture and srv
    D3D11_TEXTURE2D_DESC desc = {};
    m_albedoAtlasTexture->GetDesc(&desc);
    desc.MipLevels = 1;
    desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

    THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, m_tempAtlasTexture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    
    THROW_IF_FAILED(device->CreateShaderResourceView(m_tempAtlasTexture.Get(), &srvDesc, m_tempAtlasSRV.GetAddressOf()));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    THROW_IF_FAILED(device->CreateUnorderedAccessView(m_tempAtlasTexture.Get(), &uavDesc, m_tempAtlasUAV.GetAddressOf()));

    THROW_IF_FAILED(device->CreateShaderResourceView(m_albedoAtlasTexture.Get(), &srvDesc, m_albedoAtlasSRV.GetAddressOf()));

    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    THROW_IF_FAILED(device->CreateShaderResourceView(m_depthAtlasTexture.Get(), &srvDesc, m_depthAtlasSRV.GetAddressOf()));
}

void ImpostorBaker::PrepareBake(ID3D11DeviceContext* context)
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(m_albedoAtlasRTV.Get(), clearColor);
	context->ClearDepthStencilView(m_depthAtlasDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, m_albedoAtlasRTV.GetAddressOf(), m_depthAtlasDSV.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
	context->RSSetState(m_rasterizerState.Get());
}

void ImpostorBaker::Bake(ID3D11DeviceContext* context, const GraphicsComponent* graphicsComponent, const Batch& batch)
{
	SetRenderTargets(context);
	SetDepthStencilState(context);
	SetRasterizerState(context);
	SetShaders(context);

	float framesMinusOne = (float)ms_atlasFramesCount - 1;

	const auto& boundingBox = graphicsComponent->GetBoundingBox();
	const Vector4f& center = boundingBox.m_center;
	auto lookat = DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f);
	float radius = boundingBox.GetRadius();
	float diameter = radius * 2.0f;

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
		auto globalUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

        if (DirectX::XMComparisonAllTrue(DirectX::XMVector4EqualR(DirectX::XMVector3Cross(xmRay, globalUp), DirectX::XMVectorZero())))
            globalUp = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(xmvecPos, lookat, globalUp);
		DirectX::XMMATRIX projMatrix = DirectX::XMMatrixOrthographicLH(diameter, diameter, 0.0f, diameter);

		UpdateViewProjMatrix(context, viewMatrix, projMatrix);
		SetViewProjMatrixBuffer(context);

		SetViewport(context, x, y);

		context->Draw(batch.verticesCount, 0);
	}

	//THROW_IF_FAILED(DirectX::SaveWICTextureToFile(context, m_albedoAtlasTexture.Get(), GUID_ContainerFormatPng, L"AlbedoImpostorAtlas.png"));
    //DoProcessing(context);
}

void ImpostorBaker::DoProcessing(ID3D11DeviceContext* context)
{
    context->CSSetShader(m_maskingCS.Get(), nullptr, 0);

    context->CSSetShaderResources(0, 1, m_albedoAtlasSRV.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, m_tempAtlasUAV.GetAddressOf(), nullptr);

    const uint32_t GROUP_SIZE = 256;
    const uint32_t MAX_DIM_GROUPS = 1024;
    const uint32_t MAX_DIM_THREADS = (GROUP_SIZE * MAX_DIM_GROUPS);
    const uint32_t length = ms_atlasDimension * ms_atlasDimension;

    uint32_t x, y, z;
    if (length <= MAX_DIM_THREADS)
    {
        x = (length - 1) / GROUP_SIZE + 1;
        y = z = 1;
    }
    else
    {
        x = MAX_DIM_GROUPS;
        y = (length - 1) / MAX_DIM_THREADS + 1;
        z = 1;
    }

    context->Dispatch(x, y, z);
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
	context->OMSetRenderTargets(1, m_albedoAtlasRTV.GetAddressOf(), m_depthAtlasDSV.Get());
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