add_library(ptest
    test.c
)
target_include_directories(ptest PUBLIC
    include
)
