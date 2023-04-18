add_library(SemanticAnalyzer "")


target_sources(SemanticAnalyzer 
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/STTypeVisitor.cpp
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/STTypeVisitor.hpp
)

target_include_directories(SemanticAnalyzer
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)