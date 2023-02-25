
add_executable(lexer-test lexer_test.cpp ../include/acutest.h)

target_link_libraries(lexer-test
    PRIVATE
    Lexer
)


add_test(
  NAME
    test_invalid_file
  COMMAND
    $<TARGET_FILE:lexer-test> bad_file
  )

add_test(
  NAME
    test_valid_file
  COMMAND
    $<TARGET_FILE:lexer-test> good_file
  WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests
  )

add_test(
  NAME
    test_working_files
  COMMAND
    $<TARGET_FILE:lexer-test> good_files
  WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests
  )
