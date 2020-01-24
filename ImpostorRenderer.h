#pragma once
#include "pch.h"

class Renderer;
class GraphicsComponent;

class ImpostorRenderer
{
public:
    static void Initialize(Renderer* renderer);

    static void Render(GraphicsComponent* graphicComponent);

private:
    static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    static Microsoft::WRL::ComPtr <ID3D11PixelShader> m_ps;
};