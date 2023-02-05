#pragma once

#include "token.hpp"

#include <string>
#include <fstream>
#include <sstream>

class Lexer {

        // Used to display error information
        std::string fileName;
        int lineNumber;
        int columnNumber;

        // actual file
        std::ifstream sourceFile;

        // line stream
        std::stringstream lineStream;

        // token buffer
        std::stringstream tokenBuffer;


        public:
        Lexer(std::string file_path) :
            fileName(file_path),
            lineNumber(0),
            columnNumber(0),
            sourceFile(file_path),
            lineStream(),
            tokenBuffer()
        {

        };


        // helper methods
        void nextLine();        // this doesn't return but rather replaces the member tokenBuffer
        void skipWhiteSpace();
        // Accessor methods
        Token getNextToken();
};