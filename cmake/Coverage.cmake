function(enable_coverage target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|.*Clang")
        option(ENABLE_COVERAGE "Enable coverage reporting" FALSE)
        if(ENABLE_COVERAGE)
            target_compile_options(${target} PUBLIC
                --coverage
                -g2 -O0 -fno-omit-frame-pointer
            )
            target_link_libraries(${target} PUBLIC
                --coverage
            )
        endif()
    else()
        message(AUTHOR_WARNING "Unsupported toolchain '${CMAKE_C_COMPILER_ID}' for code coverage")
    endif()
endfunction()
