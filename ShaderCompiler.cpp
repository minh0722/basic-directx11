#include "ShaderCompiler.h"
#include <string>

using Microsoft::WRL::ComPtr;

HRESULT __stdcall D3DInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
    std::string filePath;

    switch (IncludeType)
    {
    case D3D_INCLUDE_LOCAL:
        filePath = m_shaderDir + "\\" + pFileName;
        break;
    case D3D_INCLUDE_SYSTEM:
        filePath = m_systemDir + "\\" + pFileName;
        break;
    default:
        return E_FAIL;
    }

    std::ifstream includeFile(filePath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    if (!includeFile.is_open())
        return E_FAIL;
    
    u32 fileSize = includeFile.tellg();
    char* buf = new char[fileSize];
    includeFile.seekg(0, std::ios::beg);
    includeFile.read(buf, fileSize);
    includeFile.close();
    *ppData = buf;
    *pBytes = fileSize;

    return S_OK;
}

HRESULT __stdcall D3DInclude::Close(LPCVOID pData)
{
    delete[](char*)pData;
    return S_OK;
}

ComPtr<ID3DBlob> ShaderCompiler::CompileShader(const char* filePath, const std::vector<const char*> shaderDefines)
{
    ComPtr<ID3DBlob> compiledBlob;

    const u32 definesCount = shaderDefines.size();
    std::vector<D3D_SHADER_MACRO> defines;
    defines.resize(definesCount);
    for (u32 i = 0; i < definesCount; ++i)
    {
        defines[i] = { shaderDefines[i], "" };
    }


    //D3DCompileFromFile((LPCWSTR)filePath, defines.data(), )
}