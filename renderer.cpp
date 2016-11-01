#include "pch.h"
#include "renderer.h"

Renderer::Renderer()
{
}

void Renderer::Initialize(HWND window)
{
	m_Window = window;

	initSwapChain(window);
	initVertexShader();
	initPixelShader();
}

void Renderer::render()
{	
	m_DeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	D3D11_INPUT_ELEMENT_DESC vertexShaderInputLayout[2];
	vertexShaderInputLayout[0].SemanticName = "POSITION";
	vertexShaderInputLayout[0].SemanticIndex = 0;								// will use POSITION0 semantic
	vertexShaderInputLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
	vertexShaderInputLayout[0].InputSlot = 0;									// 0 ~ 15
	vertexShaderInputLayout[0].AlignedByteOffset = 0;
	vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
	vertexShaderInputLayout[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

	vertexShaderInputLayout[1].SemanticName = "COLOR";
	vertexShaderInputLayout[1].SemanticIndex = 0;
	vertexShaderInputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexShaderInputLayout[1].InputSlot = 0;
	vertexShaderInputLayout[1].AlignedByteOffset = sizeof(Position);
	vertexShaderInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexShaderInputLayout[1].InstanceDataStepRate = 0;

	ID3DBlob* vertexBlob;
	THROW_IF_FAILED(D3DReadFileToBlob(L"vertexShader.cso", &vertexBlob));

	THROW_IF_FAILED(
		m_Device->CreateInputLayout(
			vertexShaderInputLayout, 
			2, 
			vertexBlob->GetBufferPointer(), 
			vertexBlob->GetBufferSize(), 
			m_VertexInputLayout.GetAddressOf()));

	m_DeviceContext->IASetInputLayout(m_VertexInputLayout.Get());

	m_DeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	m_DeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	m_DeviceContext->DrawIndexed(3, 0, 0);
}

void Renderer::initVertexShader()
{
	ID3DBlob* blob;
	THROW_IF_FAILED(D3DReadFileToBlob(L"vertexShader.cso", &blob));

 	THROW_IF_FAILED(
		m_Device->CreateVertexShader(
			blob->GetBufferPointer(), 
			blob->GetBufferSize(), 
			nullptr, 
			m_VertexShader.GetAddressOf()));
	
}

void Renderer::initPixelShader()
{
	ID3DBlob* blob;
	THROW_IF_FAILED(D3DReadFileToBlob(L"pixelShader.cso", &blob));

	THROW_IF_FAILED(
		m_Device->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_PixelShader.GetAddressOf()));
}

void Renderer::initSwapChain(HWND window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 1080;
	swapChainDesc.BufferDesc.Height = 720;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// the window for the swap chain
	swapChainDesc.OutputWindow = m_Window;

	// turn the multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// set to windowed mode
	swapChainDesc.Windowed = true;


	// discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// don't set the advanced flag
	swapChainDesc.Flags = 0;

	const D3D_FEATURE_LEVEL feature[] = { D3D_FEATURE_LEVEL_11_1 };

	THROW_IF_FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			feature,
			ARRAYSIZE(feature),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_SwapChain.GetAddressOf(),
			m_Device.GetAddressOf(),
			nullptr,
			m_DeviceContext.GetAddressOf()));
}

void Renderer::initVertexBuffer()
{
	Vertex v[3];

	v[0].pos = { -1.0, -1.0f, 0.0f, 0.0f };		// bottom left
	v[0].color = { 1.0f, 0.0f, 0.0f, 0.0f };
	
	v[1].pos = { 1.0, -1.0f, 0.0f, 0.0f };		// bottom right
	v[1].color = { 1.0f, 0.0f, 0.0f, 0.0f };

	v[2].pos = { 0.0f, 1.0f, 0.0f, 0.0f };		// top middle
	v[2].color = { 1.0f, 0.0f, 0.0f, 0.0f };


	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = 3 * sizeof(Vertex);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(Vertex);
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = v;

	THROW_IF_FAILED(
		m_Device->CreateBuffer(
			&desc, 
			&initData, 
			m_VertexBuffer.GetAddressOf()));
}

void Renderer::initIndexBuffer()
{
	uint32_t indices[3] = { 0, 1, 2 };

	D3D11_BUFFER_DESC desc = {};

	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = 3 * sizeof(uint32_t);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(uint32_t);
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices;

	THROW_IF_FAILED(
		m_Device->CreateBuffer(
			&desc,
			&initData,
			m_IndexBuffer.GetAddressOf()));
}
