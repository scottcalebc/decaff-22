#include <iostream>

#include "lexer.hpp"

#define VERSION 0.1.0

int main(int argc, char** argv) {
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        std::cerr << "  file_path    -   path to source file to convert" << std::endl;

        return 1;
    }

    std::string file_path(argv[1]);

    Lexer lexer = Lexer(file_path);
    Token token = Token();
    do {
        try {
            token = lexer.getNextToken();

            std::cout << token << std::endl;
        } 
        catch( ... ) {
            std::cout << "ERROR" << std::endl;
        }
    } while (token.type != Token::Type::END);


    return 0;
}
