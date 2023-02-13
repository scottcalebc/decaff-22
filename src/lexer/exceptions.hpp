#include <exception>
#include <string>

#include "token.hpp"

namespace Scanner {
    class GenericException : public std::exception {
        std::string message;
        
        public:
        GenericException(char * msg) : 
            message(msg)
        {};

        GenericException(const int lineNumber, std::string msg )
        {
            std::stringstream ss;
            ss << "\n*** Error line " << lineNumber << "." << std::endl;
            
            if (!msg.empty())
                ss << msg;
            
            message = ss.str();
        };

        const char * what()  {
            return message.c_str();
        }
    };

    class UnterminatedString : public GenericException {

        public:
        UnterminatedString(const int lineNumber, std::string tokenString) :
            GenericException{ lineNumber, "*** Unterminated string constant: " + tokenString + "\n" }
        {

        };
    };

    class IdentifierTooLong: public GenericException {
        public:
        IdentifierTooLong(const int lineNumber, std::string tokenString) :
            GenericException{ lineNumber, "*** Identifier too long: \"" + tokenString + "\"\n" }
        {

        };
    };

    class UnrecognizedCharacter: public GenericException {
        public:
        UnrecognizedCharacter(const int lineNumber, std::string tokenString) :
            GenericException{ lineNumber, "*** Unrecognized char: \'" + tokenString + "\'\n" }
        {

        };
    };
}