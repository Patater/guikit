include(../cmake/Sanitizers.cmake)
include(../cmake/Coverage.cmake)
include(../cmake/Warnings.cmake)

add_library(guikit
    hash.c
    panic.c
)

target_include_directories(guikit
    PUBLIC ../include
)

target_compile_features(guikit PUBLIC c_std_90)

enable_sanitizers(guikit)
enable_coverage(guikit)
enable_warnings(guikit)
