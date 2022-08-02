set(SRC
    src/glad.c
)

add_library(glad STATIC ${SRC})

target_include_directories(glad
    PUBLIC
        include
)
