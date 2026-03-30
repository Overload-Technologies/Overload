cmake_minimum_required(VERSION 3.16)
project(Overload LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Read project version and define OVERLOAD_VERSION
file(READ "${CMAKE_SOURCE_DIR}/VERSION.txt" OVERLOAD_VERSION_CONTENT)
string(STRIP "${OVERLOAD_VERSION_CONTENT}" OVERLOAD_VERSION_CONTENT)
if(NOT OVERLOAD_VERSION_CONTENT)
  set(OVERLOAD_VERSION_CONTENT "0.0")
endif()
add_compile_definitions(OVERLOAD_VERSION=\"${OVERLOAD_VERSION_CONTENT}\")

# Define global compile definitions that premake set
add_compile_definitions(TRACY_ENABLE TRACY_ON_DEMAND TRACY_MEMORY_ENABLE LUA_SCRIPTING GRAPHICS_API_OPENGL)

# Output directories similar to premake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin-Int)

# Add Sources where engine targets live
add_subdirectory(Sources)

# Optionally add dependencies that provide CMakeLists
if(EXISTS ${CMAKE_SOURCE_DIR}/Dependencies)
  file(GLOB dep_dirs RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Dependencies/*)
  foreach(dep ${dep_dirs})
    if(EXISTS ${CMAKE_SOURCE_DIR}/Dependencies/${dep}/CMakeLists.txt)
      add_subdirectory(${CMAKE_SOURCE_DIR}/Dependencies/${dep})
    endif()
  endforeach()
endif()
