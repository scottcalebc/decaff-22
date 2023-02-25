#include "acutest.h"
#include "lexer.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>


using namespace Scanner;

void helper_fake_main(std::string file)
{
    std::string filepath(file);
    Lexer lexer(filepath);

}

void test_invalid_file(void)
{
    TEST_EXCEPTION(helper_fake_main("bad-file.decaf"),std::exception); 
}

void test_valid_file(void)
{
    helper_fake_main("./samples/program.decaf"); 
}

void test_working_files(void)
{
    std::vector<std::string> file_names = {
        "program.decaf",
        "reserve_op.frag",
        "ident.frag"
    };

    for ( auto & file : file_names) 
    {
        helper_fake_main("./samples/"+file);
    }
}


TEST_LIST = {
    { "bad_file", test_invalid_file},
    { "good_file", test_valid_file},
    { "good_files", test_working_files},
    { NULL, NULL }

};
