solution "TP"
	configurations { "debug", "release" }

		local gkit_files =
		{
			"gKit/*.cpp", "gKit/*.h",
			"gKit/GL/*.cpp", "gKit/GL/*.h",
			"gKit/Widgets/*.cpp", "gKit/Widgets/*.h"
		}

		includedirs
		{
			".",
			"gKit",
		}

		defines { "GK_OPENGL4", "VERBOSE" }

		configuration "debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "release"
--~ 			defines { "NDEBUG" }
			flags { "OptimizeSpeed" }

		configuration "linux"
			buildoptions { "-mtune=native" }
			buildoptions { "-W -Wall -Wno-unused-parameter", "-pipe" }

			local glew_include= "local/linux32/include"
			local glew_lib= "local/linux32/lib"

			local sdl2_include= "local/linux32/include"
			local sdl2_lib= "local/linux32/lib"
--~ 			local glew_include= "$(HOME)/local/glew-1.10.0/include"
--~ 			local glew_lib= "$(HOME)/local/glew-1.10.0/lib"

--~ 			local sdl2_include= "$(HOME)/sources/SDL/include"
--~ 			local sdl2_lib= "$(HOME)/sources/SDL/lib"

			includedirs { sdl2_include, glew_include }

			linkoptions { "-L" .. glew_lib }
			linkoptions { "-Wl,-rpath," .. glew_lib}

			linkoptions { "-L" .. sdl2_lib }
			linkoptions { "-Wl,-rpath," .. sdl2_lib }

--~ 			defines { "GK_OPENEXR" }
--~ 			includedirs { "/usr/include/OpenEXR" }
--~ 			links{ "IlmImf", "IlmThread", "Imath", "Half" }

			links { "GLEW", "SDL2", "SDL2_image", "SDL2_ttf" }
			links { "GL" }

		configuration { "linux", "release" }
--~ 			buildoptions { "-fopenmp" }
--~ 			linkoptions{ "-fopenmp" }
			
		configuration { "linux", "debug" }
			buildoptions { "-mtune=native -Og" }

		configuration "windows"
			includedirs { "local/windows/include" }
			libdirs { "local/windows/lib32" }
			defines { "WIN32", "NVWIDGETS_EXPORTS", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
			defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((
			links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image", "SDL2_ttf" }

-- pour mac osX, avant premake : le plus simple est d'installer macports (http://www.macports.org/) pour glew et OpenEXR
-- Apres installation : sudo port install OpenEXR, sudo port install glew
-- En cas d'utilisation de Homebrew, remplacer "/opt/local" par "/usr/local" dans "includedirs" et "libdirs"
		configuration "macosx"
			defines { "APPLE_OSX" }
			defines { "GK_OPENEXR" }

			includedirs { "/opt/local/include", "/opt/local/include/OpenEXR" }
			libdirs { "/opt/local/lib" }
			links { "IlmImf", "IlmThread", "Imath", "Half" }

			buildoptions { "-framework OpenGL -framework Cocoa -framework SDL2 -framework SDL2_image -framework SDL2_ttf" }
			linkoptions { "-framework OpenGL -framework Cocoa -framework SDL2 -framework SDL2_image -framework SDL2_ttf" }
			links { "GLEW" }



local project_files = {
	"tutorial1",
	"tutorial2",
	"tutorial3",
	"image_viewer",
	"mesh_viewer",
	"shader_kit",
	"compute_kit",
	"filter_tutorial1",
	"filter_compute_tutorial1",
	"tp1", 
	"tp1sphere_reyes",
	"tp1shader",
	"batch_perf",
	"tessellation_tuto",
	"tessellation_sphere",
	"compute_tutorial1",
	"compute_tutorial2",
	"compute_tutorial3",
}

for i, name in ipairs(project_files) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		files ( gkit_files )
		files { name..'.cpp' }
end

-- squelette d'application GL3 core profile sans utiliser gKit
project("mini_gl3core")
	language "C++"
	kind "ConsoleApp"
	files { "mini_gl3core.cpp" }


