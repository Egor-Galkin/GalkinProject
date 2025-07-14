cmake_minimum_required(VERSION 3.31)
project(GalkinApp)

set(CMAKE_CXX_STANDARD 20)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

add_executable(GalkinApp
        src/main.cpp
        src/converterjson.cpp
        src/searchserver.cpp
        src/invertedindex.cpp
)

target_include_directories(GalkinApp PRIVATE include)

target_link_libraries(GalkinApp PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(GalkinAppTest)
