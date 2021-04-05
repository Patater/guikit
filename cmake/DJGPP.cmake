set(CMAKE_SYSTEM_NAME DOS)
set(triple i586-pc-msdosdjgpp-gcc)
set(CMAKE_C_COMPILER ${triple})
set(CMAKE_CXX_COMPILER ${triple})
set(CMAKE_CROSSCOMPILING_EMULATOR "dosbox")

# GUI Kit configuration
set(GRAPHICS "VGA" CACHE STRING "Graphics rendering backend")
