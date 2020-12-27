#include "pch.h"

#include <Windowsx.h>
#include "systemclass.h"
#include "imgui_impl_win32.h"

SystemClass::SystemClass()
{
    Initialize();
}

SystemClass::~SystemClass()
{
    Shutdown();
}

void SystemClass::Initialize()
{
	// Initialize the width and height of the screen to zero before sending the variables into the function.
	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
    THROW_IF_NULL(m_Input);

	// Initialize the input object.
	m_Input->Initialize();

	// Initialize the renderer
    m_Renderer = &Renderer::GetInstance();
	m_Renderer->Initialize(m_hwnd);
}

void SystemClass::Shutdown()
{
	if(m_Input)
	{
		delete m_Input;
	}

	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg = {};
	bool done, result;
			
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
		m_Renderer->Render(m_Input);
        m_Input->ResetMouseWheel();
        m_Input->ResetPanningDirection();
        m_Input->ResetRotatingDirection();
	}
}

bool SystemClass::Frame()
{
	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    static bool isPanning = false;
    static bool isCtrl = false;
    static bool isRotating = false;

	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);

            if ((unsigned int)wparam == VK_CONTROL)
            {
                isCtrl = true;
            }

			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);

            if ((unsigned int)wparam == VK_CONTROL)
            {
                isCtrl = false;
                isRotating = false;
                OUTPUT_DEBUG("Rotating = false\n");
            }

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			//OUTPUT_DEBUG("Left mouse down\n");
			return 0;
		}

		case WM_LBUTTONUP:
		{
			//OUTPUT_DEBUG("Left mouse up\n");
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			//OUTPUT_DEBUG("Right mouse down\n");
			return 0;
		}

		case WM_RBUTTONUP:
		{
			//OUTPUT_DEBUG("Right mouse up\n");
			return 0;
		}

		case WM_MBUTTONDOWN:
		{
            int xpos = GET_X_LPARAM(lparam);
            int ypos = GET_Y_LPARAM(lparam);
            Vector2<int> pos(xpos, ypos);

            if(!isCtrl)
            {
                isPanning = true;
                m_Input->SetPanningPosition(pos);
            }
            else
            {
                isRotating = true;
                m_Input->SetRotatingPosition(pos);
                OUTPUT_DEBUG("Rotating = true\n");
            }

			//OUTPUT_DEBUG("Scroller mouse down\n");
			return 0;
		}

		case WM_MBUTTONUP:
		{
            if(!isCtrl)
            {
                isPanning = false;
                m_Input->ResetPanningPosition();
            }
            else
            {
                isRotating = false;
                m_Input->ResetRotatingPosition();
            }

            //OUTPUT_DEBUG("Scroller mouse up\n");
            return 0;
		}

		case WM_MOUSEMOVE:
		{
            int xpos = GET_X_LPARAM(lparam);
            int ypos = GET_Y_LPARAM(lparam);
            Vector2<int> currPos(xpos, ypos);
			
            if (isPanning)
            {
                const Vector2<int> lastPanningPos = m_Input->GetPanningPosition();
                Vector2<int> panDir = lastPanningPos - currPos;
				panDir[1] = -panDir[1];
				
                m_Input->SetPanningDirection(panDir);
                m_Input->SetPanningPosition(currPos);
            }
            else if (isRotating)
            {
                const Vector2<int> lastRotatingPos = m_Input->GetRotatingPosition();
                Vector2<int> rotDir = lastRotatingPos - currPos;
                rotDir[0] = -rotDir[0];
                rotDir[1] = -rotDir[1];

                m_Input->SetRotatingDirection(rotDir);
                m_Input->SetRotatingPosition(currPos);
            }

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			int roll = GET_KEYSTATE_WPARAM(wparam);
			int zdelta = GET_WHEEL_DELTA_WPARAM(wparam);
			m_Input->SetMouseWheelDelta(zdelta);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int screenWidth, int screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;


	// Get an external pointer to this object.	
	g_ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = (LPCSTR)m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	//// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	//if(FULL_SCREEN)
	//{
	//	// If full screen set the screen to maximum size of the users desktop and 32bit.
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	//	dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
	//	dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
	//	dmScreenSettings.dmBitsPerPel = 32;			
	//	dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//	// Change the display settings to full screen.
	//	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

	//	// Set the position of the window to the top left corner.
	//	posX = posY = 0;
	//}
	//else
	//{
		// If windowed then set it to 800x600 resolution.
		//screenWidth  = 1080;
		//screenHeight = 720;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	//}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME, (LPCSTR)m_applicationName, (LPCSTR)m_applicationName,
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	//// Fix the display settings if leaving full screen mode.
	//if(FULL_SCREEN)
	//{
	//	ChangeDisplaySettings(NULL, 0);
	//}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass((LPCSTR)m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	g_ApplicationHandle = NULL;

	return;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
        return true;

	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return g_ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}