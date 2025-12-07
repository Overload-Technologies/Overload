cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myapp main.cpp)

target_include_directories(myapp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include
)

target_compile_features(myapp PRIVATE cxx_std_20)