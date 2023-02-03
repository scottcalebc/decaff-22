add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(lexer)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    lexer
)