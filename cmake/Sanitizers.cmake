function(enable_sanitizers target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|.*Clang")
        option(ENABLE_SANITIZERS "Enable some sanitizers" FALSE)
        if(ENABLE_SANITIZERS)
            set(GUIKIT_SANITIZERS
                -fsanitize=address
                -fsanitize=undefined
                -g2
            )
            target_compile_options(${target} PUBLIC
                ${GUIKIT_SANITIZERS}
            )
            target_link_libraries(${target} PUBLIC
                ${GUIKIT_SANITIZERS}
            )
        endif()
    else()
        message(AUTHOR_WARNING "Unsupported toolchain '${CMAKE_C_COMPILER_ID}' for sanitizers")
    endif()
endfunction()
