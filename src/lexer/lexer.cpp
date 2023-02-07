#include <regex>
#include <iostream>
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
    std::string spaceChars("\t\n");
    char incomingChar;

    lineStream.get(incomingChar);
    std::cout << "Attempting to take whitespace out..." << std::endl;
    std::cout << "Starting character: " << incomingChar << std::endl;
    
    // TODO : This does not eat space characters only tabs and newlines
    while (spaceChars.find(incomingChar) != std::string::npos)
    {
        lineStream.get(incomingChar);
    }

    // whichever character was taken from string that broke loop needs to be put back
    lineStream.putback(incomingChar);
    std::cout << "Putting character back: " << incomingChar << std::endl;
    std::cout << "Line is now: " << lineStream.str() << std::endl;

    // reduce to loop
    

}

Token Lexer::getNextToken()
{
    // if we read new line and it's all white space/empty line then we need to go 
    // ahead and read next line
    std::cout << "Testing if line is empty" << std::endl;
    while(lineStream.str().empty()) {
        std::cout << "Empty line getting line which should be :" << std::endl;
        nextLine();
        std::cout << "line [" << lineNumber << "] : " << lineStream.str() << std::endl;
        skipWhiteSpace();
        // takeWhile(isWhiteSpace());

        // skip comments as well here
    }

    // skip WhiteSpace here for each continued line read
    skipWhiteSpace();

    std::cout << "Starting token reading..." << std::endl;

    // clear token buffer before progressing
    tokenBuffer.str(std::string());

    // beginning of token generation
    char tmp;
    lineStream.get(tmp);

    std::cout << "Starting token character: " << tmp << std::endl;

    // find out what type of token it might be with beginning char
    Token token = Token();
    tokenBuffer << tmp;
    token.lineNumber = lineNumber;
    token.colStart = columnNumber;
    // if it starts with a character its likely an identifier
    if ( isAlpha()(tmp) )
    {

        token.type = Token::Type::Identifier;
        std::cout << "Found Identifier..." << std::endl;

        // take while isIdentifier
        takeWhile(isIdentifier());

        // compare identifier with keywords using map
        auto op = Token::keywords.find(tokenBuffer.str());
        if (op != Token::keywords.end())
        {
            std::cout << "Token string is keyword found type: " << Token::getTypeName(op->second) << std::endl;
            token.type = op->second;
        }
    }
    // if it starts with a number it is either Int or DoubleConst
    else if ( isNumber()(tmp) )
    {
        token.type = Token::Type::IntConstant;
        takeWhile(isNumber());
    }
    else if ( isOperator()(tmp) )
    {
        token.type = Token::Type::Operator;

        // need to peek to see if next character is also an operator
        tmp = lineStream.peek();
        if ( isOperator()(tmp) ) 
        {
            std::string cmpString(tokenBuffer.str() + tmp);

            // if operator exists in map insert into buffer and adjust type
            auto op = Token::operators.find(cmpString);
            if (op != Token::operators.end())
            {
                tokenBuffer << tmp;
                token.type = op->second;
            }
        }
    }
    else if ( isSeparator()(tmp) )
    {
        token.type = Token::Type::Separator;
    }
    else if ( tmp == '\"')
    {
        
        token.type = Token::Type::StringConstant;
        // take while string constant
        takeWhile(isNotStringConstantEnd());
        
        if (tokenBuffer.str().back() != '\"')
        {
            // Throw exception with Token
            throw(1);
        }
        
    }
    else
    {
        // invalid symbol for token throw generic error
        throw(1);
    }

    token.value = tokenBuffer.str();

    return token;
}