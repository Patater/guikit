function(enable_warnings target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|.*Clang")
        option(ENABLE_WARNINGS "Enable some compiler warnings" FALSE)
        if(ENABLE_WARNINGS)
            set(GUIKIT_CFLAGS
                -Wall
                -Werror
                -Wextra
                -std=c89
                -pedantic
            )
            target_compile_options(${target} PUBLIC
                ${GUIKIT_CFLAGS}
            )
            target_link_libraries(${target} PUBLIC
                ${GUIKIT_CFLAGS}
            )
        endif()
    else()
        message(AUTHOR_WARNING "Unsupported toolchain '${CMAKE_C_COMPILER_ID}' for compiler warnings")
    endif()
endfunction()
