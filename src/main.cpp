#include <iostream>
#include <unistd.h>
#include "lexer.hpp"
#include "exceptions.hpp"

#define VERSION 0.1.0

using namespace Scanner;

int main(int argc, char** argv) {

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        std::cerr << "  file_path    -   path to source file to convert" << std::endl;

        return 1;
    }

    std::string file_path(argv[1]);

    Scanner::Lexer lexer(file_path);
    Scanner::Token token;
    do {
        try {
            // std::cout << "Getting Token..."<< std::endl;
            token = lexer.getNextToken();

            std::cout << token;
            // exit(1);
            // sleep(1);
        } 
        catch( Scanner::GenericException &exc ) {
            // std::cout << std::endl;
            std::cout << exc.what() << std::endl;
            // std::cout << std::endl;
            token.type = Token::Type::ERROR;
        }

    } while (token.type != Token::Type::END);


    return 0;
}
