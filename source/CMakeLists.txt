
set(SRC
    Bot.cpp
    Bot.h

    BotNeuralNet.cpp
    BotNeuralNet.h

    SDL.cpp
    SDL.h

    GUI.cpp
    GUI.h

    Settings.h
    Systems.h

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

    Organics.cpp
    Organics.h

    Rock.cpp
    Rock.h

    Apple.cpp
    Apple.h

    MyTypes.cpp
    MyTypes.h

    Settings.h
    Systems.h

    ObjectSaver.cpp
    ObjectSaver.h
)

find_package(OpenGL REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SRC})

target_include_directories(${CMAKE_PROJECT_NAME} 
    PUBLIC
        ${IMGui_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    PUBLIC
        ${IMGui_LIBRARIES}
        ${OPENGL_LIBRARIES}
        SDL2_ttf::SDL2_ttf-static
        imgui
        implot
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
