add_library(Tree "")

target_link_libraries(Tree Token)

target_sources(Tree
    PRIVATE
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/AbstractSyntaxTree.hpp
)

target_include_directories(Tree
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/..
  ${CMAKE_CURRENT_LIST_DIR}
)