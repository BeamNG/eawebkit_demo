root = path.getabsolute(".")

_ACTIONSTR = _ACTION or 'unknown'

solution ("WebkitTest")
  configurations { "Debug", "Release" }
  location("build/" .. _ACTIONSTR .. '/')
  objdir("build/" .. _ACTIONSTR .. '/obj')
  platforms { 'x86', 'x64' }
  symbols "On"
  buildoptions {
      '/wd4018 /wd4100 /wd4121 /wd4127 /wd4130 /wd4244 /wd4302 /wd4311 /wd4312 /wd4245 /wd4389 /wd4511 /wd4512 /wd4800 /wd4995 /wd4458 /wd4456 /wd4091',
      '/nologo /GF /Qpar- /fp:except- /GR  /Zo', -- /Gy-
  }
  characterset 'Unicode'
  flags {
      'NoMinimalRebuild',
      'MultiProcessorCompile',
      'NoNativeWChar',
      'StaticRuntime', -- /MT instead of /MD
  }
  floatingpoint 'fast'
  exceptionhandling 'On'

  defines {
    '_CRT_SECURE_NO_DEPRECATE',
    '_CRT_SECURE_NO_WARNINGS',
    --'_CRT_NONSTDC_NO_DEPRECATE',
  }
  startproject ("DX11Viewer")

if _OPTIONS["toolset"] ~= '' then
  toolset(_OPTIONS["toolset"])
end

  filter "platforms:x86"
    architecture "x86"

  filter "platforms:x64"
    architecture "x86_64"

  filter {"platforms:x86", "kind:ConsoleApp or WindowedApp or SharedLib"}
    targetdir ('Bin32')

  filter {"platforms:x64", "kind:ConsoleApp or WindowedApp or SharedLib"}
    targetdir ('Bin64')

  filter "configurations:Debug"
    targetsuffix ".debug"
    optimize "Debug"
    defines {
      '_DEBUG',
      'BGE_DEBUG',
      'BGE_ENABLE_ASSERTS',
      --'BGE_DEBUG_GFX_MODE',
    }
  filter "configurations:Release"
    optimize "Speed"
    defines {
      'NDEBUG'
    }
    buildoptions {
      '/O2 /Ob2 /Oi /Ot /GT'
    }
    flags {
      'NoBufferSecurityCheck',
      'NoFramePointer',
    }

-------------------------------------------------------------------------------
-- Projects

include('DX11Viewer')