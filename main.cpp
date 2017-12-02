#include "pch.h"
#include <iostream>

#include "renderer.h"
#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system = new SystemClass;
	system->Run();
	delete system;


	return 0;
}