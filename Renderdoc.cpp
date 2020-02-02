#include "pch.h"
#include "Renderdoc.h"

RENDERDOC_API_1_4_0* Renderdoc::ms_renderdoc = nullptr;

void Renderdoc::Init()
{
	if (HMODULE mod = LoadLibrary("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&ms_renderdoc);
		assert(ret == 1);

		ms_renderdoc->SetCaptureFilePathTemplate("renderdoc_captures/capture");
	}
}

void Renderdoc::BeginCapture()
{
	if (ms_renderdoc) 
		ms_renderdoc->StartFrameCapture(nullptr, nullptr);
}

void Renderdoc::EndCapture()
{
	if (ms_renderdoc) 
		ms_renderdoc->EndFrameCapture(nullptr, nullptr);
}