#include "ShaderCompiler.h"
#include <string>

#define ENABLE_DEBUG_SYMBOLS 0

using Microsoft::WRL::ComPtr;

HRESULT __stdcall D3DInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
    std::string filePath;

    switch (IncludeType)
    {
    case D3D_INCLUDE_LOCAL:         // include with ""
        filePath = m_shaderDir + "\\" + pFileName;
        break;
    case D3D_INCLUDE_SYSTEM:        // include with <>
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

// ShaderCompiler
ShaderCompiler::ShaderCompiler()
{
    m_Include = new D3DInclude("../../../", "../../../");
}

ComPtr<ID3DBlob> ShaderCompiler::CompileShader(const char* filePath, const std::vector<const char*> shaderDefines, const char* entryPoint, ShaderType shaderType)
{
    // setting macros
    const u32 definesCount = shaderDefines.size();
    std::vector<D3D_SHADER_MACRO> defines;
    defines.resize(definesCount + 1);
    for (u32 i = 0; i < definesCount; ++i)
    {
        defines[i] = { shaderDefines[i], "" };
    }
    defines[definesCount] = { NULL, NULL };

    const char* target = nullptr;
    switch (shaderType)
    {
    case ShaderType::CS:
        target = "cs_5_0";
        break;
    case ShaderType::VS:
        target = "vs_5_0";
        break;
    case ShaderType::PS:
        target = "ps_5_0";
        break;
    case ShaderType::GS:
        target = "gs_5_0";
        break;
    case ShaderType::DS:
        target = "ds_5_0";
        break;
    case ShaderType::HS:
        target = "hs_5_0";
        break;
    default:
        THROW_IF_FALSE(0);
    }

#if ENABLE_DEBUG_SYMBOLS
    u32 compileOptions = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    u32 compileOptions = 0;
#endif

    ComPtr<ID3DBlob> compiledBlob;
    ComPtr<ID3DBlob> errors;

    HRESULT hr = D3DCompileFromFile((LPCWSTR)filePath, defines.data(), m_Include, entryPoint, target, compileOptions, 0, compiledBlob.GetAddressOf(), errors.GetAddressOf());
    THROW_IF_FAILED(hr);

    return compiledBlob;
}