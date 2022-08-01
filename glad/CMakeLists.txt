set(SRC
    src/glad.c
)

add_library(glad STATIC ${SRC})

target_include_directories(glad
    PUBLIC
        include
)

#target_link_libraries(${CMAKE_PROJECT_NAME} 
#    PUBLIC
#        ${IMGui_LIBRARIES}
#        ${SDL2_LIBRARIES}
#        ${SDL2TTF_LIBRARY}
#        imgui_sdl
#)