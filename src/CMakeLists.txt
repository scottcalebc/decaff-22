add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(lexer)
add_subdirectory(token)
add_subdirectory(parser)
add_subdirectory(tree)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Lexer
    Parser
)