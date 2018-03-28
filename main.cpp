#include "pch.h"
#include <iostream>

#include "renderer.h"
#include "systemclass.h"

#include "ObjLoader.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    wavefront::Obj result = wavefront::ObjLoader::Parse("../../../assets/Models/spaceCraft6.obj");

	SystemClass* system = new SystemClass;
	system->Run();
	delete system;
	 

	return 0;
}