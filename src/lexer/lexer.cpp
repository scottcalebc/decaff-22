#include <regex>
#include "lexer.hpp"
#include "predicates.hpp"


void Lexer::nextLine()
{
    std::string tmp;
    std::getline(sourceFile, tmp);
    lineStream << tmp;

    // increment line number
    lineNumber++;
}

/**
 * @brief Helper function to eat whitespace before token reading
 * 
 */
void Lexer::skipWhiteSpace()
{
    std::string spaceChars(" \t\n");
    char incomingChar;

    lineStream >> incomingChar;
    
    while (spaceChars.find(incomingChar) != std::string::npos)
    {
        lineStream >> incomingChar;
    }

    // whichever character was taken from string that broke loop needs to be put back
    lineStream.put(incomingChar);

}

Token Lexer::getNextToken()
{
    // if we read new line and it's all white space/empty line then we need to go 
    // ahead and read next line
    while(lineStream.str().empty()) {
        nextLine();
        skipWhiteSpace();

        // skip comments as well here
    }

    // beginning of token generation
    char tmp;
    lineStream >> tmp;

    // find out what type of token it might be with beginning char
    Token token = Token();
    // if it starts with a character its likely an identifier
    if ( isAlpha()(tmp) )
    {
        // take while isIdentifier
    }
    // if it starts with a number it is either Int or DoubleConst
    else if ( isNumber()(tmp) )
    {
        
    }
    else if ( isOperator()(tmp) )
    {
        // load operator into token buffer first
        tokenBuffer << tmp;
        token.lineNumber = lineNumber;
        token.colStart = columnNumber;
        token.type = Token::Type::Operator;

        // need to peek to see if next character is also an operator
        tmp = lineStream.peek();
        if ( isOperator()(tmp) ) 
        {
            std::string cmpString(tokenBuffer.str() + tmp);

            if (cmpString.compare("&&") == 0)
            {
                tokenBuffer << tmp;
                token.type = Token::Type::And;
            }

            if (cmpString.compare("||") == 0) 
            {
                tokenBuffer << tmp;
                token.type = Token::Type::Or;
            }

            if (cmpString.compare("<=") == 0) 
            {
                tokenBuffer << tmp;
                token.type = Token::Type::LessEqual;
            }

            if (cmpString.compare(">=") == 0)
            {
                tokenBuffer << tmp;
                token.type = Token::Type::LessEqual;
            }

            if (cmpString.compare("==") == 0)
            {
                tokenBuffer << tmp;
                token.type = Token::Type::Equal;
            }
            
            if (cmpString.compare("!=") == 0)
            {
                tokenBuffer << tmp;
                token.type = Token::Type::NotEqual;
            }
            
        }
    }
    else if ( isSeparator()(tmp) )
    {
        tokenBuffer << tmp;
        token.lineNumber = lineNumber;
        token.colStart = columnNumber;
        token.type = Token::Type::Separator;
    }
    else if ( tmp == '\"')
    {
        // take while string constant
    }
    else
    {
        throw(1);
    }

    return token;
}