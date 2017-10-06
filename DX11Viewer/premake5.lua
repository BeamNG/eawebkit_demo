project ("DX11Viewer")
  kind "WindowedApp"
  flags {
    'WinMain',
  }
  links {
    'd3d11.lib',
    'd3dcompiler.lib',
    'ws2_32.lib'
  }
  defines {
  }
  includedirs {
    '.',
  }
  libdirs {
  }
  files {
    "**.h",
    "**.rc",
    "**.cpp",
    "**.c",
  }
