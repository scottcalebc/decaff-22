#pragma once

#include <locale>

class isAlpha
{
    public:
    bool operator() (const char &c) const
    {
        std::locale locale;
        
        return std::isalpha(c, locale);
    }
};

class isIdentifier
{
    public:
    bool operator() (const char &c) const
    {
        std::locale locale;
        
        return std::isalnum(c, locale) || c == '_';
    }
};

class isNumber
{
    public:
    bool operator() (const char &c) const
    {
        std::locale locale;
        
        return std::isdigit(c, locale);
    }
};

/*
    This check is only for possible single char operators
*/
class isOperator
{
    public:
    bool operator() (const char &c) const
    {
        std::string operators("+-*/%<>=!&|");

        return operators.find(c) != std::string::npos;
    }
};

/*
    This check is only for double char operators, while some operators
    like >=, ==, etc are not included here because they have single char
    representations, thus their collection will be done in map
*/
class isOnlyDoubleCharOperator
{
    public:
    bool operator() (const char &c) const
    {
        std::string operators("&|");
        return operators.find(c) != std::string::npos;
    }
};

class isSeparator
{
    public:
    bool operator() (const char &c) const
    {
        std::string operators(";.,{}()");

        return operators.find(c) != std::string::npos;
    }
};

class isMatchingCharacters
{
    std::string characters;

    public:
    isMatchingCharacters(std::string characters) :
        characters(characters)
    {

    };

    bool operator() (const char &c) const
    {
        return characters.find(c) != std::string::npos;
    };
};

class isNotMatchingCharacters
{
    std::string characters;

    public:
    isNotMatchingCharacters(std::string characters) :
        characters(characters)
    {

    };

    bool operator() (const char &c) const
    {
        return characters.find(c) == std::string::npos;
    };
};

class isNotStringConstantEnd : public isNotMatchingCharacters
{
    public:
    isNotStringConstantEnd() :
        isNotMatchingCharacters{"\"\n"}
    {
    };
};

class isNotCommentEnd : public isNotMatchingCharacters
{
    public:
    isNotCommentEnd() :
        isNotMatchingCharacters{"/"}
    {
    };
};

class isWhiteSpace : public isMatchingCharacters
{
    public:
    isWhiteSpace() :
        isMatchingCharacters{" \t\n"}
    {
    };
};




