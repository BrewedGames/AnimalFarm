function link_to(lib)
   links (lib)
   includedirs ("./vendor/"..lib.."/include")
   libdirs ("./vendor/"..lib.."/lib" )
end



workspace "StarStruck"
   configurations { "Debug", "Release" }
   architecture "x86_64"

project "StarStruck"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   includedirs{"./helpers"}
   includedirs { "src" }
   includedirs { "C:/StarStruck/MathLibrary/include"}
   includedirs {"C:/StarStruck/SDL/include"}
   includedirs {"C:/StarStruck/OpenGL/include"}
   includedirs {"C:/StarStruck/lua/include"}
   includedirs {"C:/StarStruck/ini/include"}
   includedirs {"C:/StarStruck/sol2/include"}
   includedirs {"C:/StarStruck/cute/include"}
   libdirs {"C:/StarStruck/SDL/lib"}
   libdirs {"C:/StarStruck/OpenGL/lib"}
   libdirs {"C:/StarStruck/lua/lib"}
   links ("SDL2")
   links ("SDL2_image")
   links ("SDL2_mixer")
   links ("SDL2_ttf")
   links ("SDL2main")
   links ("SDL2test")
   links {"opengl32"}
   links {"glew32d"}
   links {"glew32sd"}
   links {"lua54"}
   cppdialect "C++17"
   
   filter "system:windows"
      buildoptions { "/bigobj" }

   files { "**.h", "**.cpp", "**.lua" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
