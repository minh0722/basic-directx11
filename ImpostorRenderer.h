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

    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_vertexDataSRV;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexDataBuffer;

    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_vsConstants;
    static Microsoft::WRL::ComPtr<ID3D11Buffer> m_psConstants;
};