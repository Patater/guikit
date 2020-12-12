include(../cmake/Sanitizers.cmake)
include(../cmake/Coverage.cmake)
include(../cmake/Warnings.cmake)

add_library(guikit
    hash.c
    hashmap.c
    panic.c
    pmemory.c
)

target_include_directories(guikit
    PUBLIC ../include
)

if(GRAPHICS STREQUAL "SDL2")
    find_package(SDL2 REQUIRED)
    target_link_libraries(guikit PUBLIC
        ${SDL2_LIBRARIES}
    )
    target_sources(guikit PRIVATE
        sdl2/graphics.c
    )
    target_include_directories(guikit PRIVATE
        ${SDL2_INCLUDE_DIRS}
    )
else()
    target_sources(guikit PRIVATE
        dos/graphics.c
    )
endif()

target_compile_features(guikit PUBLIC c_std_90)

enable_sanitizers(guikit)
enable_coverage(guikit)
enable_warnings(guikit)
