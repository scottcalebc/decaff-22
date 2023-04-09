add_library(Visitor "")


target_sources(Visitor 
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/parseVisitor.cpp
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/parseVisitor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/astVisitor.hpp

)

target_include_directories(Visitor
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)
