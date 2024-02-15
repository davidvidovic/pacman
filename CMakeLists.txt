cmake_minimum_required(VERSION 2.8)
project(pacman)

#set(OpenCV_DIR /home/daconi/build)

find_package(OpenCV REQUIRED)

# Hardcode OpenMP flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

include_directories(${OpenCV_INCLUDE_DIRS})

# Adjust the path to your source file (assuming it's in the src directory)
set(SOURCE_DIR src)
set(SOURCES ${SOURCE_DIR}/pacman.cpp)

add_executable(pacman ${SOURCES})
target_link_libraries(pacman ${OpenCV_LIBS} -fopenmp)

# Add a custom target for cleaning
add_custom_target(clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/pacman
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
)

