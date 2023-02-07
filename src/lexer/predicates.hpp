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
        
        return std::isalnum(c, locale);
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

class isOperator
{
    public:
    bool operator() (const char &c) const
    {
        std::string operators("+-*/%<>=|!");

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

class isNotStringConstantEnd : public isMatchingCharacters
{
    public:
    isNotStringConstantEnd() :
        isMatchingCharacters{ "\"\n"}
    {
    };
};

class isWhiteSpace : public isMatchingCharacters
{
    public:
    isWhiteSpace() :
        isMatchingCharacters{ " \t\n"}
    {
    };
};


