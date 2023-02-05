# include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(cpp-example test.cpp ../include/acutest.h)

add_test(
  NAME
    test_example
  COMMAND
    $<TARGET_FILE:cpp-example>
  )