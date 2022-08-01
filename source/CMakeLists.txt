
set(SRC
    Bot.cpp
    Bot.h

    BotNeuralNet.cpp
    BotNeuralNet.h

    Field.cpp
    Field.h

    Main.cpp
    Main.h

    NeuralNetRenderer.cpp
    NeuralNetRenderer.h

    Neuron.cpp
    Neuron.h

    Object.cpp
    Object.h

    MyTypes.h
    Settings.h
    Systems.h
)

find_package(IMGui REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})

target_include_directories(${CMAKE_PROJECT_NAME} 
    PUBLIC
        ${IMGui_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    PUBLIC
        ${IMGui_LIBRARIES}
        SDL2_ttf::SDL2_ttf-static
        imgui_sdl
        imgui
        glad
)

if (UNIX)
    target_compile_definitions(${CMAKE_PROJECT_NAME} 
        PUBLIC 
            UNIX
    )
endif (UNIX)
