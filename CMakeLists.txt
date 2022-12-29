
cmake_minimum_required (VERSION 3.10)

project(CyberBiology VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenGL_GL_PREFERENCE    GLVND
    CACHE STRING "cmake --help-policy CMP0072")

add_subdirectory(libs)
add_subdirectory(source)
