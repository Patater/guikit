add_executable(test_hash
    hash.c
    test.c
)
target_link_libraries(test_hash guikit)
add_test(NAME hash COMMAND test_hash)

add_executable(test_hashmap
    hashmap.c
    test.c
)
target_link_libraries(test_hashmap guikit)
add_test(NAME hashmap COMMAND test_hashmap)

add_executable(test_intern
    intern.c
    test.c
)
target_link_libraries(test_intern guikit)
add_test(NAME intern COMMAND test_intern)

add_executable(test_primrect
    primrect.c
    test.c
)
target_link_libraries(test_primrect guikit)
add_test(NAME primrect COMMAND test_primrect)

add_executable(test_array
    array.c
    test.c
)
target_link_libraries(test_array guikit)
add_test(NAME array COMMAND test_array)
