#pragma once

#include <string>
#include <iomanip>
#include <exception>

#include "ParseTree.hpp"


namespace Parser
{
    class ParseException : public std::exception {
        std::string message;

        public:
            ParseException(char *msg) :
                message(msg)
                {

                };

            ParseException(Scanner::Token token )
                {
                    std::stringstream ss;
                    
                    ss << std::endl << "*** Error line " << token.lineNumber << "." << std::endl
                        << token.lineInfo << std::endl
                        << std::setw(token.colStart - 1) << " "
                        << std::setfill('^') << std::setw(token.getValue<std::string>().length()) << "" << std::endl
                        << "*** syntax error" << std::endl;
                    
                    message = ss.str();
                };

            const char * what() {
                return message.c_str();
            }
            
    };

} // namespace Parser


