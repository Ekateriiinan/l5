cmake_minimum_required(VERSION 3.15)
project(PMRStack)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)

add_library(pmr_stack
    include/fixed_block_memory_resource.hpp
    include/pmr_stack.hpp
    src/fixed_block_memory_resource.cpp
)

target_include_directories(pmr_stack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(tests tests/test_containers.cpp)
target_link_libraries(tests pmr_stack GTest::gtest GTest::gtest_main)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(demo demo/main.cpp)
target_link_libraries(demo pmr_stack)
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(tests)