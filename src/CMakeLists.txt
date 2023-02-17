add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(lexer)
add_subdirectory(token)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Lexer
)