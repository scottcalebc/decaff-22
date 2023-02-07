#pragma once

#include "token.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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

        template<typename UnaryPredicate>
        void takeWhile(UnaryPredicate predicate)
        {
            char in;
            lineStream.get(in);

            std::cout << "Reading any all tokens that match predicate..." << std::endl;

            while(predicate(in))
            {
                tokenBuffer << in;
                std::cout << "Character matches: " << in << std::endl;
                lineStream.get(in);
            }

            // whichever character was taken from string that broke loop needs to be put back
            lineStream.putback(in);

            std::cout << "Found token string: " << tokenBuffer.str() << std::endl;
        }
        template<typename UnaryPredicate>
        void takeWhile(UnaryPredicate predicate, int maxChars)
        {
            char in;
            lineStream.get(in);

            // get current length
            int numChars = tokenBuffer.str().length();

            while(numChars < maxChars && predicate(in))
            {
                tokenBuffer << in;
            }

            // whichever character was taken from string that broke loop needs to be put back
            lineStream.put(in);
        }
};