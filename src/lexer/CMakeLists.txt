add_library(Lexer "")

target_link_libraries(Lexer Token)

target_sources(Lexer
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/lexer.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lexer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/predicates.hpp
    ${CMAKE_CURRENT_LIST_DIR}/exceptions.hpp
)

target_include_directories(Lexer
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )