cmake_minimum_required(VERSION 3.2)

project(CyberBiology3)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(SDL_ttf)

find_package(SDL2 REQUIRED)

add_library(imgui_sdl STATIC
    imgui_sdl/imgui_sdl.cpp
    imgui_sdl/imgui_sdl.h
)

target_link_libraries(imgui_sdl
    PUBLIC
        ${SDL2_LIBRARIES}
)

target_include_directories(imgui_sdl 
    PUBLIC 
        imgui_sdl
        ${SDL2_INCLUDE_DIRS}
)

add_subdirectory(glad)
add_subdirectory(source)
