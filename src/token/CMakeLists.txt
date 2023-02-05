add_library(Token "")

target_sources(Token
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/token.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/token.hpp
)

target_include_directories(Token
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )