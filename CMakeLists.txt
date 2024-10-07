cmake_minimum_required(VERSION 3.10)

# Project name and version
project(RaylibStarterProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Define build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build configurations" FORCE)

# Set up Raylib
# Adjust these paths based on where Raylib is located
set(RAYLIB_DIR "/raylib") # Adjust path accordingly
include_directories(include ${RAYLIB_DIR} ${RAYLIB_DIR}/external)

# Define source files
set(SOURCES
    src/main.cpp
    src/mouse.cpp
    src/namespaces.cpp
    src/path.cpp
    src/soldier.cpp
    src/tile.cpp
    src/world.cpp
    src/effects.cpp
    src/graph.cpp
    src/node.cpp
)

# Define header files (optional, but useful for IDEs)
set(HEADERS
    include/mouse.h
    include/path.h
    include/namespaces.h
    include/soldier.h
    include/tile.h
    include/world.h
    include/effects.h
    include/graph.h
    include/node.h
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add libraries for Linux, adjusting based on the actual needs of your project
if (UNIX)
    target_link_libraries(${PROJECT_NAME} pthread m dl raylib GL X11)
endif()

# Platform-specific options (here we assume x64 on Linux)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
endif()

# Debug Configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fsanitize=address")
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)

# Release Configuration
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)

# Add include directories and libraries
target_include_directories(${PROJECT_NAME} PRIVATE include ${RAYLIB_DIR} ${RAYLIB_DIR}/external)

# Link Raylib and additional libraries
target_link_libraries(${PROJECT_NAME} raylib)

