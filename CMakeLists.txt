cmake_minimum_required(VERSION 2.8)
project(pacman)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(pacman pacman.cpp)
target_link_libraries(pacman ${OpenCV_LIBS})

# Add a custom target for cleaning
add_custom_target(clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/pacman
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
)

