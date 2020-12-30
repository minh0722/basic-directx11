#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.h"
#include "renderer.h"

class SystemClass
{
public:
	SystemClass();
	~SystemClass();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    void Initialize();
    void Shutdown();

	bool Frame();
	void InitializeWindows(int, int);
	void ShutdownWindows();

private:
	TCHAR* m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input = nullptr;
	Renderer* m_Renderer = nullptr;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* g_ApplicationHandle = 0;