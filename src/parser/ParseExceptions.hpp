#include <exception>
#include <string>

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

            ParseException(Scanner::Token token, std::string lineInfo )
                {
                    std::stringstream ss;
                    
                    ss << std::endl << "*** Error line " << token.lineNumber << "." << std::endl
                        << lineInfo << std::endl
                        << std::setw(token.colStart - 1) << " "
                        << std::setfill('^') << std::setw(token.getValue<std::string>().length()) << "";

                };
            
    };

} // namespace Parser


