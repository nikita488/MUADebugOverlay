IG_ROOT = "D:/Dev/Alchemy/SDK"
DX_ROOT = "D:/Dev/DirectX/9.0"
MUA_SDK = "D:/Dev/Raven/MUASDK"

function setdebugpath (gamepath, exename, args)
	debugdir (path.translate(gamepath))
	debugcommand (path.translate(gamepath) .. "/" .. exename)
	debugargs (args)
end

function addcopypath (gamepath)
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.name}"' }
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}.pdb" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.basename}.pdb"' }
end

GAME_PATH = "D:\\Games\\Marvel Ultimate Alliance"

workspace "MUADebugMenu"
	configurations
	{
		"Debug",
		"Release",
	}

	location "build"
	setdebugpath (GAME_PATH, "Game.exe", "")
	addcopypath (path.join(GAME_PATH, "plugins"))

	kind "SharedLib"
	language "C++"
	targetname "MUADebugMenu"
	targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
	targetextension ".asi"

	files { "src/**.cpp", "thirdparty/**.cpp", "thirdparty/**.c", path.join(MUA_SDK, "*.cpp"), path.join(MUA_SDK, "Engine/**.cpp"), path.join(MUA_SDK, "Utils/**.cpp"), path.join(MUA_SDK, "thirdparty/**.cpp"), path.join(MUA_SDK, "thirdparty/**.c") }
	files { "src/**.h" }

	characterset ("MBCS")
	--toolset ("v141_xp")
	cppdialect "C++23"
	conformancemode "Off"
	links { "legacy_stdio_definitions", "winmm" }
	staticruntime "on"
	defines { "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE", "NOMINMAX", "IG_COMPILER_MSVC", "IG_TARGET_WIN32", "IG_TARGET_TYPE_WIN32", "IG_GFX_DX9", "IG_ALCHEMY_DLL=1", "_MUA", "FINAL_BUILD", "DLL_NAME=\"$(ProjectName)\"" }

	includedirs { path.join(MUA_SDK, "Engine"), path.join(MUA_SDK, "Utils"), path.join(IG_ROOT, "include"), MUA_SDK, path.join(MUA_SDK, "thirdparty"), path.join(MUA_SDK, "thirdparty/safetyhook"), "thirdparty" }
	libdirs { path.join(IG_ROOT, "DirectX9/lib") }
	
	links { "libIGCore", "libIGMath", "libIGDisplay", "libIGGfx", "libIGSg", "libIGUtils", "libIGAttrs" }

	filter "configurations:Debug*"
		defines { "_DEBUG" }
		symbols "full"
		optimize "off"
		runtime "debug"

	filter "configurations:Release*"
		defines { "NDEBUG" }
		symbols "on"
		optimize "speed"
		runtime "release"
		flags { "LinkTimeOptimization" }
		
project "28 Heroes"
	defines { "HERO_COUNT=28" }
	
project "52 Heroes"
	defines { "HERO_COUNT=52" }