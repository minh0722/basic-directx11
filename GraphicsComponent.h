#pragma once
#include "pch.h"
#include "BaseComponent.h"

class GraphicsComponent : public BaseComponent
{
public:
	struct GraphicsComponentDesc
	{
		ID3D11Device* device;
		LPCWSTR vertexShaderFilePath;
		LPCWSTR pixelShaderFilePath;
		std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputLayout;
	};

public:
	GraphicsComponent(const GraphicsComponent&) = delete;
	GraphicsComponent& operator=(const GraphicsComponent&) = delete;

	GraphicsComponent(const GraphicsComponentDesc& desc);

	void Render(ID3D11DeviceContext* context) override;
	void SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices) override;
	void SetVertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices) override;
	void SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology) override;

    void ChangeVertexBufferData(ID3D11DeviceContext* context, const std::vector<Vertex>& vertices);
    void ChangeIndexBufferData(ID3D11DeviceContext* context, const std::vector<uint32_t>& indices);
    void ChangeWorldViewProjBufferData(ID3D11DeviceContext* context, const WorldViewProj& worldViewProj);
protected:

    void InitWorldViewProjBuffer(ID3D11Device* device);
	void InitVertexShader(ID3D11Device* device, const LPCWSTR filePath);
	void InitPixelShader(ID3D11Device* device, const LPCWSTR filePath);
	void InitVertexInputLayout(
		ID3D11Device* device,
		const LPCWSTR filePath,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc);

private:
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;

    ComPtr<ID3D11Buffer> m_WorldViewProjBuffer;

	ComPtr<ID3D11InputLayout> m_VertexInputLayout;

	size_t m_IndicesCount;
};