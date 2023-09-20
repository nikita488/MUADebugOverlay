function setdebugpath (gamepath, exename, args)
	debugdir (path.translate(gamepath))
	debugcommand (path.translate(gamepath) .. "/" .. exename)
	debugargs (args)
end

function addcopypath (gamepath)
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.name}"' }
	postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}.pdb" "' .. path.translate(gamepath) .. "/" .. '%{cfg.buildtarget.basename}.pdb"' }
end

GAME_PATH = "D:\\Games\\Marvel Ultimate Alliance OG"

workspace "MUADebugMenu"
	configurations
	{
		"Debug",
		"Release",
	}

	location "build"
	setdebugpath (GAME_PATH, "Game.exe", "")
	addcopypath (GAME_PATH)

	kind "SharedLib"
	language "C++"
	targetname "MUADebugMenu"
	targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
	targetextension ".asi"

	files { "src/**.cpp" }
	files { "src/**.h" }

	characterset ("MBCS")
	--toolset ("v141_xp")
	links { "legacy_stdio_definitions", "winmm" }
	staticruntime "on"
	defines { "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE", "_USE_32BIT_TIME_T", "NOMINMAX", "DLL_NAME=\"$(ProjectName)\"" }

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
	defines { "MAX_HEROS=28" }
	
project "52 Heroes"
	defines { "MAX_HEROS=52" }