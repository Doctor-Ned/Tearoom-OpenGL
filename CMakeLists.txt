cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Tearoom VERSION 0.1)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# add thirdparties
include(thirdparty/thirdparty.cmake)

# add subdirectories
add_subdirectory(src)
set(SHADER_DIR "Shaders")
add_subdirectory(${SHADER_DIR})