cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Tearoom VERSION 0.1)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# add thirdparties
include(thirdparty/thirdparty.cmake)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    # CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    # if (_cpp_latest_flag_supported)
    #     add_compile_options("/std:c++latest")
    # endif()
    CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp17_flag_supported)
    if (_cpp17_flag_supported)
        add_compile_options("/std:c++17")
    endif()
    CHECK_CXX_COMPILER_FLAG("/MP" _multithreading)
    if (_multithreading)
        add_compile_options("/MP")
    endif()
endif()

# add subdirectories
add_subdirectory(src)
set(SHADER_DIR "Shaders")
add_subdirectory(${SHADER_DIR})