### options to build SDL ###
option(SDL2_DISABLE_INSTALL "" ON)
option(SDL2_DISABLE_SDL2MAIN "" ON)
option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(SDL)

###############################################################################

add_subdirectory(SDL_ttf)

###############################################################################

add_subdirectory(glad)

###############################################################################

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
		SDL2::SDL2-static
		imgui
)

target_include_directories(imgui_sdl 
    PUBLIC 
        imgui_sdl
)

