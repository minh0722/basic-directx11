#pragma once
#include "pch.h"
#include <d3dcompiler.h>
#include <d3dcommon.h>


class D3DInclude : ID3DInclude
{
public:
    /* 
    shader dir is for local files:
        #include "file.h"
    system dir is for files included like this:
        #include <file.f>
    */
    D3DInclude(const char* shaderDir, const char* systemDir) : m_shaderDir(shaderDir), m_systemDir(systemDir) {}

    HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
    HRESULT __stdcall Close(LPCVOID pData) override;

private:
    std::string m_shaderDir;
    std::string m_systemDir;
};

class ShaderCompiler
{
public:
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const char* filePath, const std::vector<const char*> shaderDefines);
};