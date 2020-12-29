workspace "basic-directx11"
    configurations {"Debug", "Release"}
    platforms {"x86", "x64"}

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

DIRECT_XTK_INC = "extern/DirectXTK/Inc/"

group "ExternLibs"

    project "DirectXTK"
        files {
            "extern/DirectXTK/Inc/Audio.h"
            ,"extern/DirectXTK/Inc/CommonStates.h"
            ,"extern/DirectXTK/Inc/DDSTextureLoader.h"
            ,"extern/DirectXTK/Inc/DirectXHelpers.h"
            ,"extern/DirectXTK/Inc/Effects.h"
            ,"extern/DirectXTK/Inc/GamePad.h"
            ,"extern/DirectXTK/Inc/GeometricPrimitive.h"
            ,"extern/DirectXTK/Inc/GraphicsMemory.h"
            ,"extern/DirectXTK/Inc/Keyboard.h"
            ,"extern/DirectXTK/Inc/Model.h"
            ,"extern/DirectXTK/Inc/Mouse.h"
            ,"extern/DirectXTK/Inc/PostProcess.h"
            ,"extern/DirectXTK/Inc/PrimitiveBatch.h"
            ,"extern/DirectXTK/Inc/ScreenGrab.h"
            ,"extern/DirectXTK/Inc/SimpleMath.h"
            ,"extern/DirectXTK/Inc/SimpleMath.inl"
            ,"extern/DirectXTK/Inc/SpriteBatch.h"
            ,"extern/DirectXTK/Inc/SpriteFont.h"
            ,"extern/DirectXTK/Inc/VertexTypes.h"
            ,"extern/DirectXTK/Inc/WICTextureLoader.h"
            --,"extern/DirectXTK/Audio/AudioEngine.cpp"
            --,"extern/DirectXTK/Audio/DynamicSoundEffectInstance.cpp"
            --,"extern/DirectXTK/Audio/SoundCommon.cpp"
            --,"extern/DirectXTK/Audio/SoundCommon.h"
            --,"extern/DirectXTK/Audio/SoundEffect.cpp"
            --,"extern/DirectXTK/Audio/SoundEffectInstance.cpp"
            --,"extern/DirectXTK/Audio/WaveBank.cpp"
            --,"extern/DirectXTK/Audio/WaveBankReader.cpp"
            --,"extern/DirectXTK/Audio/WaveBankReader.h"
            --,extern/DirectXTK/Audio/WAVFileReader.cpp"
            --,"extern/DirectXTK/Audio/WAVFileReader.h"
            ,"extern/DirectXTK/Src/AlignedNew.h"
            ,"extern/DirectXTK/Src/AlphaTestEffect.cpp"
            ,"extern/DirectXTK/Src/BasicEffect.cpp"
            ,"extern/DirectXTK/Src/BasicPostProcess.cpp"
            ,"extern/DirectXTK/Src/Bezier.h"
            ,"extern/DirectXTK/Src/BinaryReader.cpp"
            ,"extern/DirectXTK/Src/BinaryReader.h"
            ,"extern/DirectXTK/Src/CommonStates.cpp"
            ,"extern/DirectXTK/Src/ConstantBuffer.h"
            ,"extern/DirectXTK/Src/dds.h"
            ,"extern/DirectXTK/Src/DDSTextureLoader.cpp"
            ,"extern/DirectXTK/Src/DebugEffect.cpp"
            ,"extern/DirectXTK/Src/DemandCreate.h"
            ,"extern/DirectXTK/Src/DGSLEffect.cpp"
            ,"extern/DirectXTK/Src/DGSLEffectFactory.cpp"
            ,"extern/DirectXTK/Src/DualPostProcess.cpp"
            ,"extern/DirectXTK/Src/DualTextureEffect.cpp"
            ,"extern/DirectXTK/Src/EffectCommon.cpp"
            ,"extern/DirectXTK/Src/EffectCommon.h"
            ,"extern/DirectXTK/Src/EffectFactory.cpp"
            ,"extern/DirectXTK/Src/EnvironmentMapEffect.cpp"
            ,"extern/DirectXTK/Src/GamePad.cpp"
            ,"extern/DirectXTK/Src/GeometricPrimitive.cpp"
            ,"extern/DirectXTK/Src/Geometry.h"
            ,"extern/DirectXTK/Src/Geometry.cpp"
            ,"extern/DirectXTK/Src/GraphicsMemory.cpp"
            ,"extern/DirectXTK/Src/Keyboard.cpp"
            ,"extern/DirectXTK/Src/LoaderHelpers.h"
            ,"extern/DirectXTK/Src/Model.cpp"
            ,"extern/DirectXTK/Src/ModelLoadCMO.cpp"
            ,"extern/DirectXTK/Src/ModelLoadSDKMESH.cpp"
            ,"extern/DirectXTK/Src/ModelLoadVBO.cpp"
            ,"extern/DirectXTK/Src/Mouse.cpp"
            ,"extern/DirectXTK/Src/NormalMapEffect.cpp"
            ,"extern/DirectXTK/Src/PBREffect.cpp"
            ,"extern/DirectXTK/Src/PBREffectFactory.cpp"
            ,"extern/DirectXTK/Src/pch.h"
            ,"extern/DirectXTK/Src/pch.cpp"
            ,"extern/DirectXTK/Src/PlatformHelpers.h"
            ,"extern/DirectXTK/Src/PrimitiveBatch.cpp"
            ,"extern/DirectXTK/Src/ScreenGrab.cpp"
            ,"extern/DirectXTK/Src/SDKMesh.h"
            ,"extern/DirectXTK/Src/Shaders"
            ,"extern/DirectXTK/Src/SharedResourcePool.h"
            ,"extern/DirectXTK/Src/SimpleMath.cpp"
            ,"extern/DirectXTK/Src/SkinnedEffect.cpp"
            ,"extern/DirectXTK/Src/SpriteBatch.cpp"
            ,"extern/DirectXTK/Src/SpriteFont.cpp"
            ,"extern/DirectXTK/Src/TeapotData.inc"
            ,"extern/DirectXTK/Src/ToneMapPostProcess.cpp"
            ,"extern/DirectXTK/Src/vbo.h"
            ,"extern/DirectXTK/Src/VertexTypes.cpp"
            ,"extern/DirectXTK/Src/WICTextureLoader.cpp"
        }
        pchheader "pch.h"
        pchsource "extern/DirectXTK/Src/pch.cpp"
        kind "StaticLib"
        language "C++"
        cppdialect "C++14"
        includedirs {DIRECT_XTK_INC}

    group "ExternLibs/renderdoc/DLL"
        group "ExternLibs/renderdoc/DLL/drivers/IHV"
            externalproject "Intel"
                location "extern/renderdoc/renderdoc/driver/ihv/intel"
                uuid "7FCB5FC5-1DBD-4DA6-83A0-6BA4E945BDA5"
                kind "StaticLib"
                language "C++"
                cppdialect "C++17"
                configmap {
                    ["Debug"] = "Development",
                    ["Release"] = "Release"
                }

            externalproject "AMD"
                location "extern/renderdoc/renderdoc/driver/ihv/amd"
                uuid "5DE5A561-548A-4DD7-90F0-06A2B39EAE9A"
                kind "StaticLib"
                language "C++"
                cppdialect "C++17"
                configmap {
                    ["Debug"] = "Development",
                    ["Release"] = "Release"
                }

        group "ExternLibs/renderdoc/DLL/drivers/shaders"
            externalproject "spirv"
                filename "renderdoc_spirv"
                location "extern/renderdoc/renderdoc/driver/shaders/spirv"
                uuid "0AAE0AD1-371B-4A36-9ED1-80E10E960605"
                kind "StaticLib"
                language "C++"
                cppdialect "C++17"
                configmap {
                    ["Debug"] = "Development",
                    ["Release"] = "Release"
                }
                

group ""

project "basic-directx11"
    dependson "DirectXTK"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files{"src/**"}
    vpaths {
       ["Source/*"] = "*"
    }
    
    filter {"files:**.hlsl"}
        flags "ExcludeFromBuild"

    links {"DirectXTK"}
    includedirs {DIRECT_XTK_INC}