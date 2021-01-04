workspace "basic-directx11"
    configurations {"Debug", "Release"}
    platforms {"x86", "x64"}

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

DIRECT_XTK_INC = "extern/DirectXTK/Inc/"
RENDERDOC_INC = "extern/renderdoc/renderdoc/"
FASTCRC_INC = "extern/fastcrc32/"
IMGUI_INC = "extern/imgui/"
IMGUI_EXAMPLES_INC = "extern/imgui/examples/"

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
        targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"
        objdir "obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"

    group "ExternLibs/renderdoc"
        group "ExternLibs/renderdoc/DLL"

            -- main renderdoc dll project
            externalproject "renderdoc"
                location "extern/renderdoc/renderdoc"
                uuid "E2B46D67-90E2-40B6-9597-72930E7845E5"
                kind "SharedLib"
                language "C++"
                cppdialect "C++17"
                configmap {
                    ["Debug"] = "Development",
                    ["Release"] = "Release"
                }

            group "ExternLibs/renderdoc/DLL/breakpad"
                externalproject "breakpad_common"
                    filename "common"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows"
                    uuid "EA1242CF-BB42-B1AC-9B6A-A508D96D1CB7"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "crash_generation_client"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/crash_generation"
                    uuid "EC847717-119A-2391-0477-212E1140082C"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "exception_handler"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/handler"
                    uuid "B7399F39-300F-450E-F471-9490F959D2A7"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "crash_generation_server"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/crash_generation"
                    uuid "7893E300-3ED0-7F4C-158F-67EA63934C57"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

            group "ExternLibs/renderdoc/DLL/drivers"
                externalproject "d3d11"
                    filename "renderdoc_d3d11"
                    location "extern/renderdoc/renderdoc/driver/d3d11"
                    uuid "F1E59A05-60D4-4927-9E57-DD191EAE90EF"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "d3d12"
                    filename "renderdoc_d3d12"
                    location "extern/renderdoc/renderdoc/driver/d3d12"
                    uuid "9E6B10A2-84B4-434D-ABDB-43BE4EA650F4"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "d3d8"
                    filename "renderdoc_d3d8"
                    location "extern/renderdoc/renderdoc/driver/d3d8"
                    uuid "9C4487E8-EEB0-4A7F-BD81-23F81CD24E22"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "d3d9"
                    filename "renderdoc_d3d9"
                    location "extern/renderdoc/renderdoc/driver/d3d9"
                    uuid "44044776-9469-4079-B587-ABFFF6574AA4"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "dxgi"
                    filename "renderdoc_dxgi"
                    location "extern/renderdoc/renderdoc/driver/dxgi"
                    uuid "2A793574-BD3C-46D4-9788-C339D9550CE1"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "gl"
                    filename "renderdoc_gl"
                    location "extern/renderdoc/renderdoc/driver/gl"
                    uuid "F92FCDA6-A261-4EEC-9CD0-73A11FBCC459"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

                externalproject "vulkan"
                    filename "renderdoc_vulkan"
                    location "extern/renderdoc/renderdoc/driver/vulkan"
                    uuid "88C5DAC6-30A0-4CFD-AF51-540A977D1F3F"
                    kind "StaticLib"
                    language "C++"
                    cppdialect "C++17"
                    configmap {
                        ["Debug"] = "Development",
                        ["Release"] = "Release"
                    }

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

                    externalproject "NV"
                        location "extern/renderdoc/renderdoc/driver/ihv/nv"
                        uuid "40349AD9-5558-4DF4-84E2-11934DE90A11"
                        kind "StaticLib"
                        language "C++"
                        cppdialect "C++17"
                        configmap {
                            ["Debug"] = "Development",
                            ["Release"] = "Release"
                        }

                group "ExternLibs/renderdoc/DLL/drivers/shaders"
                    externalproject "dxbc"
                        filename "renderdoc_dxbc"
                        location "extern/renderdoc/renderdoc/driver/shaders/dxbc"
                        uuid "C43FF27E-A155-4852-88EC-5CE9334C07A8"
                        kind "StaticLib"
                        language "C++"
                        cppdialect "C++17"
                        configmap {
                            ["Debug"] = "Development",
                            ["Release"] = "Release"
                        }

                    externalproject "dxil"
                        filename "renderdoc_dxil"
                        location "extern/renderdoc/renderdoc/driver/shaders/dxil"
                        uuid "8AE46EC2-EA46-441F-BEE2-94097101D6A3"
                        kind "StaticLib"
                        language "C++"
                        cppdialect "C++17"
                        configmap {
                            ["Debug"] = "Development",
                            ["Release"] = "Release"
                        }

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

        group "ExternLibs/renderdoc/Utility"
            externalproject "version"
                filename "renderdoc_version"
                location "extern/renderdoc/renderdoc"
                uuid "257FD75C-4D17-4A23-A754-23BFD85887A0"
                kind "StaticLib"
                language "C++"
                cppdialect "C++17"
                configmap {
                    ["Debug"] = "Development",
                    ["Release"] = "Release"
                }

group ""

project "basic-directx11"
    dependson {"DirectXTK", "renderdoc", "imgui"}
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    includedirs { DIRECT_XTK_INC, RENDERDOC_INC, FASTCRC_INC, IMGUI_INC, IMGUI_EXAMPLES_INC }
    links {"DirectXTK", "renderdoc", "D3DCompiler", "D3D11.lib", "DXGI.lib"}
    targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"
    objdir "obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"
    debugdir "bin/%{cfg.platform}/%{cfg.buildcfg}"
    linkoptions "/SUBSYSTEM:WINDOWS"

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files{
        "src/**",

        -- imgui
        "extern/imgui/*.h",
        "extern/imgui/*.cpp",
        "extern/imgui/examples/imgui_impl_dx11.h",
        "extern/imgui/examples/imgui_impl_dx11.cpp",
        "extern/imgui/examples/imgui_impl_win32.h",
        "extern/imgui/examples/imgui_impl_win32.cpp",

        -- fastcrc32
        "extern/fastcrc32/C3c32.h",
        "extern/fastcrc32/Crc32.cpp"
    }

    vpaths {
       ["Source/*"] = "src/*",
       ["3rdparty/imgui"] = "extern/imgui/*",
       ["3rdparty/fastcrc32"] = "extern/fastcrc32/*"
    }
    
    filter {"files:**.hlsl"}
        flags "ExcludeFromBuild"

    filter {"files:extern/**"}
        flags "NoPCH"