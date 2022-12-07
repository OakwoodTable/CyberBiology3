
cmake_minimum_required (VERSION 3.10)

project(CyberBiology VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#Sources
set(SOURCE_FILES

    imgui/imgui.cpp
    imgui/imgui.h

    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp

    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_sdl.cpp
    imgui/backends/imgui_impl_sdl.h
    imgui/backends/imgui_impl_sdlrenderer.cpp
    imgui/backends/imgui_impl_sdlrenderer.h

    implot/implot.cpp
    implot/implot.h
    implot/implot_internal.h

    implot/implot_demo.cpp
    implot/implot_items.cpp

    source/SDL.cpp
    source/SDL.h

    source/GUI.cpp
    source/GUI.h

    source/Settings.h
    source/Systems.h

    source/Bot.cpp
    source/Bot.h

    source/BotNeuralNet.cpp
    source/BotNeuralNet.h

    source/Field.cpp
    source/Field.h

    source/Main.cpp
    source/Main.h

    source/NeuralNetRenderer.cpp
    source/NeuralNetRenderer.h

    source/Neuron.cpp
    source/Neuron.h

    source/Object.cpp
    source/Object.h

    source/Organics.cpp
    source/Organics.h

    source/Rock.cpp
    source/Rock.h

    source/Apple.cpp
    source/Apple.h

    source/MyTypes.h
    source/MyTypes.cpp

    source/ObjectSaver.cpp
    source/ObjectSaver.h

)

#Executable, target Windows
add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCE_FILES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)




#lib files
#target_link_libraries(${CMAKE_PROJECT_NAME} 
#opengl32.lib 
#${CMAKE_CURRENT_SOURCE_DIR}/SDL/lib/SDL2.lib)

#target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
#imgui 
#imgui/backends 
#implot 
#SDL/include)


#static libs

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
imgui 
imgui/backends 
implot )

option(SDL2_DISABLE_INSTALL "" ON)
option(SDL2_DISABLE_SDL2MAIN "" ON)
option(SDL2_DISABLE_UNINSTALL  "" ON)
option(BUILD_SHARED_LIBS "" OFF)

add_subdirectory(SDL)

target_link_libraries(${CMAKE_PROJECT_NAME} 
opengl32.lib 
SDL2-static)


#output to /bin folder
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)


