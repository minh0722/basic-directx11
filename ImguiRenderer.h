#pragma once
#include "pch.h"

class ImguiRenderer
{
public:
    ImguiRenderer() = default;
    ~ImguiRenderer();
    
    void Initialize(HWND _window, ID3D11Device* _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mainRenderTarget);
    void Deinit();

    void Render();

    void ShowMainMenu();

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_mainRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
};