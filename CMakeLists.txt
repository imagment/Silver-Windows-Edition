cmake_minimum_required(VERSION 3.10)

# Project configuration
project(Silver VERSION 0.5.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Detect compiler type
if(MSVC)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+ API
endif()

# Windows-specific threading (NO PTHREAD)
if(WIN32)
    add_definitions(-DUSE_WINDOWS_THREADS)
endif()

# Paths for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Precompiled header setup
set(PCH_HEADER ${CMAKE_SOURCE_DIR}/include/Silver.hpp)
set(PCH_OUTPUT_DIR ${CMAKE_BINARY_DIR}/pch)
set(PCH_COMPILED ${PCH_OUTPUT_DIR}/Silver_pch.hpp.gch)

file(MAKE_DIRECTORY ${PCH_OUTPUT_DIR})

add_custom_command(
    OUTPUT ${PCH_COMPILED}
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -x c++-header -o ${PCH_COMPILED} ${PCH_HEADER}
    DEPENDS ${PCH_HEADER}
    COMMENT "Compiling precompiled header..."
)

# ---------- Library: Silver ----------
file(GLOB_RECURSE SILVER_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(Silver STATIC ${SILVER_SRC})

# Precompiled header for Silver library
target_compile_options(Silver PRIVATE -include ${PCH_HEADER})

# ---------- Executable: MyGame ----------
file(GLOB_RECURSE MYGAME_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(MyGame ${MYGAME_SRC})

# Precompiled header for executable
target_compile_options(MyGame PRIVATE -include ${PCH_HEADER})

# Link libraries
target_link_libraries(MyGame PRIVATE Silver winmm)  # Ensure winmm is linked

# Compiler-specific warning suppression
if(MSVC)
    target_compile_options(MyGame PRIVATE /W0)
else()
    target_compile_options(MyGame PRIVATE -w)
endif()
