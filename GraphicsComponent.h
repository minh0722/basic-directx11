#pragma once
#include "pch.h"
#include "ObjLoader.h"
#include "MaterialLoader.h"

struct Batch
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    UINT verticesCount;
    UINT vertexBufferStride;
    UINT indicesCount;
};

class GraphicsComponent
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

	void Render(ID3D11DeviceContext* context, bool isInstanceRendering = false, uint32_t instanceCount = 1);
    void BakeImpostor(ID3D11DeviceContext* context);
	void SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices);
	void SetIndexBuffer(ID3D11Device* device, const void* indices, size_t indicesCount);

    template <typename VertexBufferType>
    void SetVertexBuffer(ID3D11Device* device, const std::vector<VertexBufferType>& vertices);
    template <typename VertexBufferType>
    void AddVertexBatch(ID3D11Device* device, const std::vector<VertexBufferType>& vertices, uint32_t materialID);

    void AddMaterial(ID3D11Device* device, uint32_t materialID, wavefront::Material material);

	void SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology);
    void SetSamplerState(ID3D11DeviceContext* context);
    void SetDrawType(wavefront::DrawType drawType);
    void SetBoundingBox(const wavefront::AABB& boundingBox);
    void SetWorldPosition(Vector4f pos);
    void LoadTexture(ID3D11Device* device, const wchar_t* texturePath);

    //void ChangeVertexBufferData(ID3D11DeviceContext* context, const std::vector<Vertex>& vertices);
    //void ChangeIndexBufferData(ID3D11DeviceContext* context, const std::vector<uint32_t>& indices);
    void ChangeWorldViewProjBufferData(ID3D11DeviceContext* context, const WorldViewProj& worldViewProj);

    void InitSamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC desc);
protected:

    void InitWorldViewProjBuffer(ID3D11Device* device);
	void InitVertexShader(ID3D11Device* device, const LPCWSTR filePath);
	void InitPixelShader(ID3D11Device* device, const LPCWSTR filePath);
	void InitVertexInputLayout(
		ID3D11Device* device,
		const LPCWSTR filePath,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;          // for non batched geometry
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_WorldViewProjBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_VertexInputLayout = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture = nullptr;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState = nullptr;

    std::unordered_map<uint32_t, Batch> m_Batches;
    std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_MaterialBuffers;

    UINT m_IndicesCount = 0;        // for non batched geometry
    UINT m_VerticesCount = 0;
    UINT m_VertexBufferStride = 0;

    wavefront::DrawType m_drawType = wavefront::DrawType::DrawIndexed;
    wavefront::AABB m_BoundingBox;
    Vector4f m_WorldPosition;
};

#include "GraphicsComponent.hpp"