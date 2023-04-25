add_library(CodeGen "")

target_link_libraries(CodeGen AST Visitor)

target_sources(CodeGen 
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/CodeGenVisitor.cpp
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/CodeGenVisitor.hpp
)

target_include_directories(CodeGen
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)