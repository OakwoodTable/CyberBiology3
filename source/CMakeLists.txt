
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

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SRC})

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
        glad
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if (UNIX)
    target_compile_definitions(${CMAKE_PROJECT_NAME} 
        PUBLIC 
            UNIX
    )
endif (UNIX)

if (WIN32)
    string(REGEX REPLACE "\\.[^.]*$" "" SDL2_LIB ${SDL2_LIBRARY})
    get_filename_component(SDL2_NAME ${SDL2_LIBRARY} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" SDL2_NAME ${SDL2_NAME})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug ${CMAKE_CURRENT_BINARY_DIR}/Release)
	file(COPY_FILE ${SDL2_LIB}${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/Debug/${SDL2_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} ONLY_IF_DIFFERENT)
    file(COPY_FILE ${SDL2_LIB}${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/Release/${SDL2_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} ONLY_IF_DIFFERENT)
    message(STATUS "COPY ${SDL2_LIB}${CMAKE_SHARED_LIBRARY_SUFFIX} -> ${CMAKE_CURRENT_BINARY_DIR}/{Debug,Release}/${SDL2_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()