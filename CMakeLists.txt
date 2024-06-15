cmake_minimum_required(VERSION 3.23)

project(guikit
    VERSION 2.0
    DESCRIPTION "Patater GUI Kit"
    LANGUAGES C)

add_subdirectory(ptest)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(tests)
endif()

# Set the default graphics rendering backend
set(GRAPHICS "SDL2" CACHE STRING "Graphics rendering backend")

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(docs)
