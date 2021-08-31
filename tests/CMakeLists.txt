include(../cmake/Sanitizers.cmake)
include(../cmake/Coverage.cmake)
include(../cmake/Warnings.cmake)

add_executable(test_hash
    hash.c
    test.c
)
target_link_libraries(test_hash PUBLIC guikit)
enable_sanitizers(test_hash)
enable_coverage(test_hash)
enable_warnings(test_hash)
add_test(NAME hash COMMAND test_hash)

add_executable(test_hashmap
    ../src/hashmap.c
    ../src/panic.c
    ../src/pmemory.c
    hashmap.c
    test.c
)
target_link_libraries(test_hashmap PUBLIC guikit)
target_include_directories(test_hashmap
    PUBLIC ../include
)
enable_sanitizers(test_hashmap)
enable_coverage(test_hashmap)
enable_warnings(test_hashmap)
add_test(NAME hashmap COMMAND test_hashmap)

add_executable(test_intern
    intern.c
    test.c
)
target_link_libraries(test_intern PUBLIC guikit)
enable_sanitizers(test_intern)
enable_coverage(test_intern)
enable_warnings(test_intern)
add_test(NAME intern COMMAND test_intern)

add_executable(test_primrect
    primrect.c
    test.c
)
target_link_libraries(test_primrect PUBLIC guikit)
enable_sanitizers(test_primrect)
enable_coverage(test_primrect)
enable_warnings(test_primrect)
add_test(NAME primrect COMMAND test_primrect)

add_executable(test_array
    array.c
    test.c
)
target_link_libraries(test_array PUBLIC guikit)
enable_sanitizers(test_array)
enable_coverage(test_array)
enable_warnings(test_array)
add_test(NAME array COMMAND test_array)
