enable_testing()

add_executable(ps5_3 main.cpp)
target_link_libraries(ps5_3 bintreeutil)
add_test(NAME TEST_BINTREE_01 COMMAND ps5_3)
