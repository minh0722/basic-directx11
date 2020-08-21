#include "ImguiRenderer.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GPUCapturer.h"

ImguiRenderer::~ImguiRenderer()
{
    Deinit();
}

void ImguiRenderer::Initialize(HWND _window, ID3D11Device* _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mainRenderTarget)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(_window);
    ImGui_ImplDX11_Init(_device, _deviceContext.Get());

    m_mainRenderTargetView = _mainRenderTarget;
    m_deviceContext = _deviceContext;
}

void ImguiRenderer::Deinit()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImguiRenderer::Render()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // all the imgui menus and items
    ShowMainMenu();

    // render the imgui elements
    ImGui::Render();
    m_deviceContext->OMSetRenderTargets(1, m_mainRenderTargetView.GetAddressOf(), nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiRenderer::ShowMainMenu()
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Sample menu 0"))
        {
            if (ImGui::MenuItem("Menu item 0"))
            {
                OutputDebugString("MenuItem0 selected");
            }
            if (ImGui::MenuItem("Menu item 1"))
            {
                OutputDebugString("MenuItem1 selected");
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Renderdoc"))
        {
            static bool activated = true;
            if (ImGui::MenuItem("Enable/Disable renderdoc overlay", nullptr, &activated))
            {
                if (activated)
                    GPUCapturer::ShowOverlay();
                else
                    GPUCapturer::HideOverlay();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}