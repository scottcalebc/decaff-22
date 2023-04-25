add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(AST)
add_subdirectory(lexer)
add_subdirectory(token)
add_subdirectory(parser)
add_subdirectory(visitor)
add_subdirectory(common)
add_subdirectory(SymbolTable)
add_subdirectory(semantic-analyzer)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    AST
    Lexer
    Parser
    Visitor
    Common
    SymbolTable
    SemanticAnalyzer
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)