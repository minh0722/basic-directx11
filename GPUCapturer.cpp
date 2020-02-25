#include "pch.h"
#include "GPUCapturer.h"
#include "api/app/renderdoc_app.h"
#include <DXProgrammableCapture.h>
#include <dxgi1_3.h>

using Microsoft::WRL::ComPtr;

static RENDERDOC_API_1_4_0* gs_renderdocCapturer = nullptr;
static ComPtr<IDXGraphicsAnalysis> gs_pixCapturer = nullptr;

void GPUCapturer::Init(CaptureType type)
{
    if(type == CaptureType::Renderdoc)
    {
        if (HMODULE mod = LoadLibrary("renderdoc.dll"))
        {
            pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
            int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&gs_renderdocCapturer);
            assert(ret == 1);

            gs_renderdocCapturer->SetCaptureFilePathTemplate("renderdoc_captures/capture");
        }
    }
    else
    {
        DXGIGetDebugInterface1(0, IID_PPV_ARGS(&gs_pixCapturer));
    }
}

void GPUCapturer::BeginCapture()
{
    if (gs_renderdocCapturer)
        gs_renderdocCapturer->StartFrameCapture(nullptr, nullptr);

    if (gs_pixCapturer)
        gs_pixCapturer->BeginCapture();
}

void GPUCapturer::EndCapture()
{
    if (gs_renderdocCapturer)
        gs_renderdocCapturer->EndFrameCapture(nullptr, nullptr);

    if (gs_pixCapturer)
        gs_pixCapturer->EndCapture();
}