
add_executable(lexer-test lexer_test.cpp ../include/acutest.h)

target_link_libraries(lexer-test
    PRIVATE
    Lexer
)

find_program(BASH_PROGRAM bash)


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

add_test(
  NAME
    test_lexer_outputs
  COMMAND
    ${BASH_PROGRAM} ${PROJECT_SOURCE_DIR}/tests/test-output.sh ${PROJECT_SOURCE_DIR} lexer --lexer
    WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests
)

add_test(
  NAME
    test_parser_outputs
  COMMAND
    ${BASH_PROGRAM} ${PROJECT_SOURCE_DIR}/tests/test-output.sh ${PROJECT_SOURCE_DIR} parser --parser
    WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests
)

add_test(
  NAME
    test_semantic_analyzer_outputs
  COMMAND
    ${BASH_PROGRAM} ${PROJECT_SOURCE_DIR}/tests/test-output.sh ${PROJECT_SOURCE_DIR} semantic --semantic-check
    WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests
)

add_test(
  NAME
    test_code_generation_outputs
  COMMAND
    ${BASH_PROGRAM} ${PROJECT_SOURCE_DIR}/tests/test-output.sh ${PROJECT_SOURCE_DIR} codegen --code-gen
    WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR}/tests/assembly-test
)
