#include <regex>
#include <iostream>
#include "lexer.hpp"
#include "predicates.hpp"
#include "exceptions.hpp"


void Scanner::Lexer::nextLine()
{
    std::string tmp;
    std::getline(sourceFile, tmp);
    lineStream.str(tmp);
    lineStream.clear();     

    // increment line number
    lineNumber++;
    // reset column number
    columnNumber = 0;
}

/**
 * @brief Helper function to eat whitespace before token reading
 * 
 */
void Scanner::Lexer::skipWhiteSpace()
{
    std::string spaceChars(" \t\n");
    char incomingChar;

    lineStream.get(incomingChar);
    // std::cout << "Attempting to take whitespace out..." << std::endl;
    // std::cout << "Starting character: " << incomingChar << std::endl;
    
    // TODO : This does not eat space characters only tabs and newlines
    while (spaceChars.find(incomingChar) != std::string::npos)
    {
        lineStream.get(incomingChar);
    }

    // whichever character was taken from string that broke loop needs to be put back
    lineStream.putback(incomingChar);
    // std::cout << "Putting character back: " << incomingChar << std::endl;
    // std::cout << "Line is now: " << lineStream.str() << std::endl;

    // reduce to loop
    

}

Scanner::Token Scanner::Lexer::getNextToken()
{
    // set token instance now so we can early out if file is at EOF
    Token token = Token();
    // clear token buffer before progressing
    tokenBuffer.str(std::string());

    // takeWhile(isWhiteSpace());
    // if we read new line and it's all white space/empty line then we need to go 
    // ahead and read next line
    // std::cout << "Testing if line is empty" << std::endl;
    // std::cout << "ASCII value of character : " << lineStream.peek() << std::endl;
    // lineStream.peek();
    while( !sourceFile.eof() && (lineStream.eof() || lineStream.peek() == -1) )  {
        // std::cout << "Empty line getting line which should be :" << std::endl;
        nextLine();
        // std::cout << "line [" << lineNumber << "] : " << lineStream.str() << std::endl;
        // skipWhiteSpace();
        takeWhile(isWhiteSpace());
        // std::cout << "Took all whitespace checking eof : " << lineStream.eof() << std::endl;

        // skip comments as well here
    }

    // early out if hit end of stream and end of file
    if ((lineStream.peek() == -1 || lineStream.eof()) && sourceFile.eof() ) {
        // std::cout << "Returning early since hit eof" << std::endl;
        return token;
        
    }
    // std::cout << "line [" << lineNumber << "] : " << lineStream.str() << std::endl;
    // skip WhiteSpace here for each continued line read
    // skipWhiteSpace();
    takeWhile(isWhiteSpace());
    tokenBuffer.str("");

    // std::cout << "Starting token reading..." << std::endl;



    // beginning of token generation
    char tmp;
    lineStream.get(tmp);
    columnNumber++;

    // std::cout << "Starting token character: " << tmp << std::endl;

    // find out what type of token it might be with beginning char
    tokenBuffer << tmp;
    token.lineNumber = lineNumber;
    token.colStart = columnNumber;
    // if it starts with a character its likely an identifier
    if ( isAlpha()(tmp) )
    {

        token.type = Token::Type::Identifier;
        // std::cout << "Found Identifier..." << std::endl;

        // take while isIdentifier
        takeWhile(isIdentifier());

        // compare identifier with keywords using map
        auto op = Token::keywords.find(tokenBuffer.str());
        if (op != Token::keywords.end())
        {
            // std::cout << "Token string is keyword found type: " << int(op->second) << std::endl;
            token.type = op->second;
        } else if (tokenBuffer.str().length() > Token::identifierMaxLength)
        {
            std::cout << IdentifierTooLong(lineNumber, tokenBuffer.str()).what() << std::endl;
        }
    }
    // if it starts with a number it is either Int or DoubleConst
    else if ( isNumber()(tmp) )
    {
        token.type = Token::Type::IntConstant;
        takeWhile(isNumber());

        if (lineStream.peek() == '.')
        {
            // possible double take period
            lineStream.get(tmp);


            // check to see if next char is a number or E
            char floatTest = lineStream.peek();
            if (isNumber()(floatTest) || floatTest == 'E' || floatTest == 'e')
            {
                // found double need to add to current token
                // we took so increment column number
                columnNumber++;

                tokenBuffer << tmp; // add '.' into buffer
                token.type = Token::Type::DoubleConstant;

                // take all characters that are numbers
                takeWhile(isNumber());
                
                // get current peek since this may be E now
                // peek will also still be the same if we no number followed decimal
                floatTest = lineStream.peek();
                if (floatTest == 'E' || floatTest == 'e')
                {
                    // new temp 
                    char tmp;
                    lineStream.get(tmp);  

                    floatTest = lineStream.peek();
                    if (isNumber()(floatTest))
                    {
                        columnNumber++;
                        tokenBuffer << tmp; // add 'E' | 'e' into buffer
                        takeWhile(isNumber());
                    }
                    else if ( floatTest == '+' || floatTest == '-')
                    {
                        // new temp 
                        char tmp2;
                        lineStream.get(tmp2);  

                        // we can now take the full double
                        if (isNumber()(lineStream.peek()))
                        {
                            columnNumber+= 2;
                            tokenBuffer << tmp;
                            tokenBuffer << tmp2;

                            takeWhile(isNumber());
                        }
                        // not proper double break out
                        else 
                        {
                            lineStream.putback(tmp2);
                            lineStream.putback(tmp);
                        }
                    }
                    // not proper doulbe break out
                    else
                    {
                        lineStream.putback(tmp);
                    }

                }

                // convert to actual value now
                // float d{0};
                // tokenBuffer >> d;

                // tokenBuffer.str(std::to_string(d));
            } 
            else
            {
                // put back period
                lineStream.putback(tmp);
            }
        }
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
                // Actually get the next token out of stream
                // and increment column number
                columnNumber++;
                lineStream.get(tmp);
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
        // reset string
        // tokenBuffer.str("");
        token.type = Token::Type::StringConstant;
        // take while string constant

        takeWhile(isNotStringConstantEnd());
        
        // takeWhile stops on qutoe, need to check if still in stream and consume
        // std::cout << "Checking to see if quote still in buffer: " << char(lineStream.peek()) << std::endl;
        if (lineStream.peek() != '\"')
        {
            // Throw exception with info
            throw UnterminatedString(lineNumber, tokenBuffer.str());
        }
        // string starts after quote increment column count
        columnNumber++;
        // std::cout << "It is so we consume it now" << std::endl;
        lineStream.get(tmp);
        tokenBuffer << tmp;
        // exit(1);
    }
    else
    {
        // invalid symbol for token throw generic error
        std::stringstream err;
        err << "Error string \'" << tmp << "\' is not a valid token";
        throw std::runtime_error(err.str());
    }

    token.value = tokenBuffer.str();

    return token;
}