#pragma once
#include "pch.h"

class Renderer;
class GraphicsComponent;

class ImpostorRenderer
{
public:
    static void Initialize(Renderer* renderer);

    static void Render(Renderer* renderer, GraphicsComponent* graphicComponent);

private:
    static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    static Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;

	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexDataBuffer;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_vsConstants;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_psConstants;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_invMatricesConstants;

    static Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    static Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};