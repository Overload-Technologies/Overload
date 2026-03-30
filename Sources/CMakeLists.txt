# Auto-generated Sources/CMakeLists.txt
# Creates a static library per subdirectory under Sources and executables for OvEditor/OvGame

cmake_minimum_required(VERSION 3.16)

# Collect dependency include directories and roots
file(GLOB DEP_DIRS RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Dependencies/*)
set(DEP_INCLUDE_DIRS)
foreach(dep ${DEP_DIRS})
  if(EXISTS ${CMAKE_SOURCE_DIR}/${dep}/include)
    list(APPEND DEP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${dep}/include)
  endif()
  if(EXISTS ${CMAKE_SOURCE_DIR}/${dep}/bullet)
    list(APPEND DEP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${dep}/bullet)
  endif()
  if(EXISTS ${CMAKE_SOURCE_DIR}/${dep}/src)
    list(APPEND DEP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${dep}/src)
  endif()
  list(APPEND DEP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${dep})
endforeach()

# Collect include directories inside each Sources module (e.g., Sources/OvCore/include)
file(GLOB SOURCE_INC_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*/include)
set(SOURCE_INCLUDE_DIRS)
foreach(sid ${SOURCE_INC_DIRS})
  list(APPEND SOURCE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${sid})
endforeach()

# Global include dirs: make Sources root, module include dirs, and Dependencies include folders available to targets
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${DEP_INCLUDE_DIRS} ${SOURCE_INCLUDE_DIRS})

# Applications that should become executables instead of libraries
set(APPS OvEditor OvGame)

# Create libraries for each module directory in Sources (skip applications)
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
    list(FIND APPS ${child} is_app)
    if(is_app EQUAL -1)
      file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.cc)
      if(srcs)
        # For some dependencies, include their source into the module so they are built
        if(${child} STREQUAL "OvPhysics")
          file(GLOB_RECURSE BULLET_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Dependencies/bullet3/bullet/*.cpp ${CMAKE_SOURCE_DIR}/Dependencies/bullet3/bullet/**/*.cpp)
          list(APPEND srcs ${BULLET_SRCS})
        endif()

        add_library(${child} STATIC ${srcs})
        target_include_directories(${child} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${child} ${CMAKE_CURRENT_SOURCE_DIR}/${child}/include ${CMAKE_CURRENT_SOURCE_DIR}/${child}/src ${DEP_INCLUDE_DIRS})
        target_compile_features(${child} PUBLIC cxx_std_20)
      endif()
    endif()
  endif()
endforeach()

# Create executables if present
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor)
  file(GLOB_RECURSE editor_srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor/*.c ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor/*.cc)
  if(editor_srcs)
    add_executable(OvEditor ${editor_srcs})
    target_include_directories(OvEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor ${CMAKE_CURRENT_SOURCE_DIR}/OvEditor/include ${DEP_INCLUDE_DIRS})
    # Link against engine libraries (these targets are created above)
    target_link_libraries(OvEditor PRIVATE OvCore OvRendering OvWindowing OvUI OvTools OvMaths OvDebug OvAudio OvPhysics)
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/OvGame)
  file(GLOB_RECURSE game_srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/OvGame/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OvGame/*.c ${CMAKE_CURRENT_SOURCE_DIR}/OvGame/*.cc)
  if(game_srcs)
    add_executable(OvGame ${game_srcs})
    target_include_directories(OvGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/OvGame ${CMAKE_CURRENT_SOURCE_DIR}/OvGame/include ${DEP_INCLUDE_DIRS})
    target_link_libraries(OvGame PRIVATE OvCore OvRendering OvWindowing OvUI OvTools OvMaths OvDebug OvAudio OvPhysics)
  endif()
endif()
