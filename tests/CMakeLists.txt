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