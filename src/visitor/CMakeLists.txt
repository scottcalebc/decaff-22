add_library(Visitor INTERFACE)

target_sources(Visitor 
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/visitor.hpp
)
