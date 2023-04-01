add_library(AST INTERFACE)


target_sources(AST 
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/AbstractSyntaxTree.hpp
)
