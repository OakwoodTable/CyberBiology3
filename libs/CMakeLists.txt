### options to build SDL ###
option(SDL2_DISABLE_INSTALL "" ON)
option(SDL2_DISABLE_SDL2MAIN "" ON)
option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(SDL)

###############################################################################

add_subdirectory(SDL_ttf)

###############################################################################

add_library(imgui STATIC
	imgui/imconfig.h
	imgui/imgui.cpp
	imgui/imgui.h
        imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h

	imgui/backends/imgui_impl_sdl.cpp
	imgui/backends/imgui_impl_sdl.h
	imgui/backends/imgui_impl_sdlrenderer.cpp
	imgui/backends/imgui_impl_sdlrenderer.h
)

target_include_directories(imgui
	PUBLIC
		imgui
		imgui/backends
)

target_link_libraries(imgui
	PUBLIC
		SDL2::SDL2-static
)

###############################################################################

add_library(implot STATIC
    implot/implot.cpp
    implot/implot.h
    implot/implot_internal.h

    implot/implot_demo.cpp
    implot/implot_items.cpp
)

target_include_directories(implot PUBLIC implot)

target_link_libraries(implot PUBLIC imgui)
