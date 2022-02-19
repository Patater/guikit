cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

find_program(iwyu NAMES include-what-you-use iwyu REQUIRED)

function(enable_iwyu target)
    option(ENABLE_IWYU "Enable checking includes with IWYU" FALSE)
    if(ENABLE_IWYU)
        set_property(TARGET ${target} PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu}
            -Xiwyu;--mapping_file=${PROJECT_SOURCE_DIR}/cmake/iwyu.map
            -Xiwyu;--quoted_includes_first
        )
    endif()
endfunction()
