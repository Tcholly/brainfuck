workspace "brainfuck"
    language "C"
    
    architecture "x86_64"
    configurations { "Debug", "Release" }

    warnings "Extra"

    filter { "configurations:Debug" }
        defines { "_DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        optimize "On"

    filter { }

    targetdir ("bin/%{prj.name}/%{cfg.longname}")
    objdir ("obj/%{prj.name}/%{cfg.longname}")

project "brainfuck"
    kind "ConsoleApp"
    files "src/**"

    includedirs {
		"src",
    }

    libdirs { }

    links { }
