workspace "basic-directx11"
    configurations {"Debug", "Release"}
    platforms {"x86", "x64"}
    startproject "basic-directx11"

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

DIRECT_XTK_INC = "extern/DirectXTK/Inc/"
RENDERDOC_INC = "extern/renderdoc/renderdoc/"
FASTCRC_INC = "extern/fastcrc32/"
IMGUI_INC = "extern/imgui/"
IMGUI_EXAMPLES_INC = "extern/imgui/examples/"

function commonRenderdocProjectSettings()
    language "C++"
    cppdialect "C++17"
    configmap {
        ["Debug"] = "Development",
        ["Release"] = "Release"
    }
end

function commonDirectorySettings()

    -- set directory of output executable and libraries
    targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"

    -- set directory of object files
    objdir "obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"

    -- set directory of output solution and project files
    location "build"
end

group "ExternLibs"

    project "DirectXTK"
        files {
            "extern/DirectXTK/Inc/**.h",
            "extern/DirectXTK/Src/**.cpp"
        }
        removefiles {
            "extern/DirectXTK/Inc/XboxDDSTextureLoader.h",
            "extern/DirectXTK/Src/XboxDDSTextureLoader.cpp"
        }

        pchheader "pch.h"
        pchsource "extern/DirectXTK/Src/pch.cpp"
        kind "StaticLib"
        language "C++"
        cppdialect "C++14"
        includedirs {DIRECT_XTK_INC}
        commonDirectorySettings()

    group "ExternLibs/renderdoc"
        group "ExternLibs/renderdoc/DLL"

            -- main renderdoc dll project
            externalproject "renderdoc"
                location "extern/renderdoc/renderdoc"
                uuid "E2B46D67-90E2-40B6-9597-72930E7845E5"
                kind "SharedLib"
                commonRenderdocProjectSettings()

            group "ExternLibs/renderdoc/DLL/breakpad"
                externalproject "breakpad_common"
                    filename "common"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows"
                    uuid "EA1242CF-BB42-B1AC-9B6A-A508D96D1CB7"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "crash_generation_client"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/crash_generation"
                    uuid "EC847717-119A-2391-0477-212E1140082C"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "exception_handler"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/handler"
                    uuid "B7399F39-300F-450E-F471-9490F959D2A7"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "crash_generation_server"
                    location "extern/renderdoc/renderdoc/3rdparty/breakpad/client/windows/crash_generation"
                    uuid "7893E300-3ED0-7F4C-158F-67EA63934C57"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

            group "ExternLibs/renderdoc/DLL/drivers"
                externalproject "d3d11"
                    filename "renderdoc_d3d11"
                    location "extern/renderdoc/renderdoc/driver/d3d11"
                    uuid "F1E59A05-60D4-4927-9E57-DD191EAE90EF"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "d3d12"
                    filename "renderdoc_d3d12"
                    location "extern/renderdoc/renderdoc/driver/d3d12"
                    uuid "9E6B10A2-84B4-434D-ABDB-43BE4EA650F4"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "d3d8"
                    filename "renderdoc_d3d8"
                    location "extern/renderdoc/renderdoc/driver/d3d8"
                    uuid "9C4487E8-EEB0-4A7F-BD81-23F81CD24E22"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "d3d9"
                    filename "renderdoc_d3d9"
                    location "extern/renderdoc/renderdoc/driver/d3d9"
                    uuid "44044776-9469-4079-B587-ABFFF6574AA4"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "dxgi"
                    filename "renderdoc_dxgi"
                    location "extern/renderdoc/renderdoc/driver/dxgi"
                    uuid "2A793574-BD3C-46D4-9788-C339D9550CE1"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "gl"
                    filename "renderdoc_gl"
                    location "extern/renderdoc/renderdoc/driver/gl"
                    uuid "F92FCDA6-A261-4EEC-9CD0-73A11FBCC459"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                externalproject "vulkan"
                    filename "renderdoc_vulkan"
                    location "extern/renderdoc/renderdoc/driver/vulkan"
                    uuid "88C5DAC6-30A0-4CFD-AF51-540A977D1F3F"
                    kind "StaticLib"
                    commonRenderdocProjectSettings()

                group "ExternLibs/renderdoc/DLL/drivers/IHV"
                    externalproject "Intel"
                        location "extern/renderdoc/renderdoc/driver/ihv/intel"
                        uuid "7FCB5FC5-1DBD-4DA6-83A0-6BA4E945BDA5"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

                    externalproject "AMD"
                        location "extern/renderdoc/renderdoc/driver/ihv/amd"
                        uuid "5DE5A561-548A-4DD7-90F0-06A2B39EAE9A"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

                    externalproject "NV"
                        location "extern/renderdoc/renderdoc/driver/ihv/nv"
                        uuid "40349AD9-5558-4DF4-84E2-11934DE90A11"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

                group "ExternLibs/renderdoc/DLL/drivers/shaders"
                    externalproject "dxbc"
                        filename "renderdoc_dxbc"
                        location "extern/renderdoc/renderdoc/driver/shaders/dxbc"
                        uuid "C43FF27E-A155-4852-88EC-5CE9334C07A8"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

                    externalproject "dxil"
                        filename "renderdoc_dxil"
                        location "extern/renderdoc/renderdoc/driver/shaders/dxil"
                        uuid "8AE46EC2-EA46-441F-BEE2-94097101D6A3"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

                    externalproject "spirv"
                        filename "renderdoc_spirv"
                        location "extern/renderdoc/renderdoc/driver/shaders/spirv"
                        uuid "0AAE0AD1-371B-4A36-9ED1-80E10E960605"
                        kind "StaticLib"
                        commonRenderdocProjectSettings()

        group "ExternLibs/renderdoc/Utility"
            externalproject "version"
                filename "renderdoc_version"
                location "extern/renderdoc/renderdoc"
                uuid "257FD75C-4D17-4A23-A754-23BFD85887A0"
                kind "StaticLib"
                commonRenderdocProjectSettings()

group ""

project "basic-directx11"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    includedirs { DIRECT_XTK_INC, RENDERDOC_INC, FASTCRC_INC, IMGUI_INC, IMGUI_EXAMPLES_INC }

    -- set project dependencies
    dependson {"DirectXTK", "renderdoc", "imgui"}

    -- set linker settings
    links {"DirectXTK", "renderdoc", "D3DCompiler", "D3D11.lib", "DXGI.lib"}
    linkoptions "/SUBSYSTEM:WINDOWS"

    commonDirectorySettings()
    debugdir "bin/%{cfg.platform}/%{cfg.buildcfg}"

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files{
        "src/**",
        "premake5.lua",

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
       ["premake/"] = "premake5.lua",
       ["3rdparty/imgui"] = "extern/imgui/*",
       ["3rdparty/fastcrc32"] = "extern/fastcrc32/*"
    }
    
    -- dont use VS built-in shader compiler
    filter {"files:**.hlsl"}
        flags "ExcludeFromBuild"

    -- extern libs dont user our precompiled header
    filter {"files:extern/**"}
        flags "NoPCH"