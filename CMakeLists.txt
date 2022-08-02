cmake_minimum_required(VERSION 3.2)

project(CyberBiology3)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(SDL_ttf)

if (UNIX)
	find_package(SDL2 REQUIRED)
else()
	# from command line -DSDL2_LIBRARY=.. -DSDL2_INCLUDE_DIR=..
	set(SDL2_LIBRARIES ${SDL2_LIBRARY})
	set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
endif()

add_library(imgui STATIC
	imgui/imconfig.h
	imgui/imgui.cpp
	imgui/imgui.h
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
)

target_include_directories(imgui
	PUBLIC
		imgui
)

add_library(imgui_sdl STATIC
    imgui_sdl/imgui_sdl.cpp
    imgui_sdl/imgui_sdl.h
)

target_link_libraries(imgui_sdl
    PUBLIC
        ${SDL2_LIBRARIES}
		imgui
)

target_include_directories(imgui_sdl 
    PUBLIC 
        imgui_sdl
        ${SDL2_INCLUDE_DIRS}
)

add_subdirectory(glad)
add_subdirectory(source)
