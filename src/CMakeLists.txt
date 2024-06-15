include(../cmake/Sanitizers.cmake)
include(../cmake/Coverage.cmake)
include(../cmake/Warnings.cmake)
include(../cmake/IWYU.cmake)

add_library(guikit
    bmp.c
    font.c
    hash.c
    hashmap.c
    histgram.c
    intern.c
    panic.c
    pmemory.c
    prandom.c
    primrect.c
)

target_sources(guikit
    PUBLIC FILE_SET HEADERS
    BASE_DIRS ../include
    FILES
        ../include/guikit/ansidos.h
        ../include/guikit/array.h
        ../include/guikit/bmp.h
        ../include/guikit/debug.h
        ../include/guikit/font.h
        ../include/guikit/graphics.h
        ../include/guikit/hash.h
        ../include/guikit/hashmap.h
        ../include/guikit/histgram.h
        ../include/guikit/intern.h
        ../include/guikit/panic.h
        ../include/guikit/pmemory.h
        ../include/guikit/prandom.h
        ../include/guikit/primrect.h
        ../include/guikit/ptypes.h
        ../include/guikit/sassert.h
        ../include/guikit/snprintf.h
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
    target_include_directories(guikit SYSTEM PRIVATE
        ${SDL2_INCLUDE_DIRS}
    )
else()
    target_sources(guikit PRIVATE
        dos/graphics.c
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "DOS")
    target_sources(guikit PRIVATE
        dos/debug.c
    )
else()
    target_sources(guikit PRIVATE
        sdl2/debug.c
    )
endif()

target_compile_features(guikit PUBLIC c_std_90)

enable_sanitizers(guikit)
enable_coverage(guikit)
enable_warnings(guikit)
enable_iwyu(guikit)
