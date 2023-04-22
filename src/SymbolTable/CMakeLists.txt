add_library(SymbolTable "")

target_link_libraries(SymbolTable AST Visitor)

target_sources(SymbolTable 
PRIVATE
${CMAKE_CURRENT_LIST_DIR}/SymbolTableVisitor.cpp
${CMAKE_CURRENT_LIST_DIR}/generate.cpp
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/SymbolTableVisitor.hpp
${CMAKE_CURRENT_LIST_DIR}/Entities.hpp
${CMAKE_CURRENT_LIST_DIR}/generate.hpp
)

target_include_directories(SymbolTable
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)