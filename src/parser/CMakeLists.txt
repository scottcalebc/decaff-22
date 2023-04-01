add_library(Parser "")

target_link_libraries(Parser Token Lexer)

target_sources(Parser
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ParseTree.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TreeGeneration.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/ParseTree.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TreeGeneration.hpp
    ${CMAKE_CURRENT_LIST_DIR}/exceptions.hpp
)

target_include_directories(Parser
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}
  )