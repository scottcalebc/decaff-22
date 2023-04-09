add_library(Common INTERFACE)


target_sources(Common 
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ParserForward.hpp
    ${CMAKE_CURRENT_LIST_DIR}/VisitorForward.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ASTForward.hpp
)

# target_include_directories(Visitor
#   PUBLIC
#   ${CMAKE_CURRENT_LIST_DIR}/..
#   ${CMAKE_CURRENT_LIST_DIR}
# )
