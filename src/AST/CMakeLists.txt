add_library(AST "")


target_sources(AST 
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ParseTreeVisitor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AbstractSyntaxTree.cpp
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/AbstractSyntaxTree.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ParseTreeVisitor.hpp
)

target_include_directories(AST
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)