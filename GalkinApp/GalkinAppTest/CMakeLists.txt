cmake_minimum_required(VERSION 3.31)
project(GalkinAppTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(gtest_disable_pthreads ON CACHE INTERNAL "")

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(GalkinAppTest
        testmain.cpp
        ../src/invertedindex.cpp
        ../src/searchserver.cpp
)

target_include_directories(GalkinAppTest PRIVATE ../include)

target_link_libraries(GalkinAppTest PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(GalkinAppTest)
