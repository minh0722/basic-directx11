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
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // all the imgui menus and items
    ShowMainMenuBar();

    // render the imgui elements
    ImGui::Render();
    m_deviceContext->OMSetRenderTargets(1, m_mainRenderTargetView.GetAddressOf(), nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiRenderer::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {        
        ShowGraphicsMenu();

        ImGui::EndMainMenuBar();
    }
}

void ImguiRenderer::ShowGraphicsMenu()
{
    if ((ImGui::BeginMenu("Graphics")))
    {
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::BeginMenu("Renderdoc"))
            {
                static bool activated = false;
                if (ImGui::MenuItem("Enable/Disable renderdoc overlay", nullptr, &activated))
                {
                    if (activated)
                        GPUCapturer::ShowOverlay();
                    else
                        GPUCapturer::HideOverlay();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Impostor"))
        {
            static bool baked = false;
            if (ImGui::MenuItem("Begin impostor bake (one time usage)", nullptr, &baked))
            {
                if (baked)
                    ;
                else
                    baked = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}