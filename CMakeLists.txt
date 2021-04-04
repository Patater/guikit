cmake_minimum_required(VERSION 3.1)

project(guikit
    VERSION 2.0
    DESCRIPTION "Patater GUI Kit"
    LANGUAGES C)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(tests)
endif()

add_subdirectory(src)
