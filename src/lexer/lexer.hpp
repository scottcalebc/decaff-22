#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <token/token.hpp>

namespace Scanner {
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
                // Throw if source is bad so we dont lock up reading
                // non-existant file
                if (!sourceFile.good())
                    throw std::invalid_argument("Invalid source file");

                // go ahead and read first line into buffer
                // nextLine();

            };


            // helper methods
            void nextLine();        // this doesn't return but rather replaces the member tokenBuffer
            void skipWhiteSpace();
            bool skipComments();
            // Accessor methods
            Token getNextToken();

            template<typename UnaryPredicate>
            void takeWhile(UnaryPredicate predicate)
            {
                char in;
                lineStream.get(in);

                // #ifdef DEBUG
                // std::cout << "Reading any all tokens that match predicate..." << std::endl;
                // #endif 

                while(!lineStream.eof() && predicate(in))
                {
                    tokenBuffer << in;
                    // #ifdef DEBUG
                    // std::cout << "Character matches: " << in << std::endl;
                    // std::cout << "Took character incrementing column count " << std::endl;
                    // #endif
                    columnNumber++;
                    lineStream.get(in);
                }

                // whichever character was taken from string that broke loop needs to be put back
                if (in != '\0') {
                    lineStream.putback(in);
                }

                #ifdef DEBUG
                std::cout << "Found token string: " << tokenBuffer.str() << std::endl;
                #endif
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
}