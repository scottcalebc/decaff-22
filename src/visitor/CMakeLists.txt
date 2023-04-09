add_library(Visitor INTERFACE)


target_sources(Visitor 
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/visitor.hpp
)

# target_include_directories(Visitor
#   PUBLIC
#   ${CMAKE_CURRENT_LIST_DIR}/..
#   ${CMAKE_CURRENT_LIST_DIR}
# )
