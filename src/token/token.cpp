#include <iomanip>
#include "token.hpp"

std::map<Scanner::Token::Type, std::string> Scanner::Token::enumName{
    {Type::If, "If"},
    {Type::Int, "Int"},
    {Type::For, "For"},
    {Type::And, "And"},
    {Type::Else, "Else"},
    {Type::Void, "Void"},
    {Type::Bool, "Bool"},
    {Type::Break, "Break"},
    {Type::While, "While"},
    {Type::String, "String"},
    {Type::Double, "Double"},
    {Type::Return, "Return"},
    {Type::Identifier, "Identifier"},
    {Type::IntConstant, "IntConstant"},
    {Type::DoubleConstant, "DoubleConstant"},
    {Type::StringConstant, "StringConstant"},
};

std::map<std::string, Scanner::Token::Type> Scanner::Token::keywords{
    {"if", Type::If},
    {"int", Type::Int},
    {"for", Type::For},
    {"else", Type::Else},
    {"void", Type::Void},
    {"bool", Type::Bool},
    {"break", Type::Break},
    {"while", Type::While},
    {"string", Type::String},
    {"double", Type::Double},
    {"return", Type::Return},
    {"null", Type::NullConstant},
    {"true", Type::BoolConstant},
    {"false", Type::BoolConstant},
};

std::map<std::string, Scanner::Token::Type> Scanner::Token::operators{
    {"!=", Type::NotEqual},
    {"==", Type::Equal},
    {">=", Type::GreaterEqual},
    {"<=", Type::LessEqual},
    {"||", Type::Or},
    {"&&", Type::And},
};

int Scanner::Token::identifierMaxLength = 32;

template<>
const std::string Scanner::Token::getValue() const
{
    return (type == Type::Identifier) ?
        value.substr(0, Token::identifierMaxLength+1) :
        value;
}

template<>
const double Scanner::Token::getValue() const
{
    return std::atof(value.c_str());
}


std::ostream& operator<<(std::ostream &out, Scanner::Token const& token)
{
    // if end token don't print anything
    if (token.type == Scanner::Token::Type::END)
        return out;

    // print token first with set spacing
    out << token.value << std::setw(20)
        << "line " << token.lineNumber << " cols " << token.colStart << "-"
        << (token.colStart + token.value.length()) - 1 << " is " ;

    switch (token.type) {
        case Scanner::Token::Type::Separator:
        case Scanner::Token::Type::Operator:
            out << "\'" << token.value << "\'";
            break;
        case Scanner::Token::Type::IntConstant:
        case Scanner::Token::Type::StringConstant:
            out << Scanner::Token::getTypeName(token.type) << " (value = " << token.value << ")";
            break;
        case Scanner::Token::Type::DoubleConstant:
            out << Scanner::Token::getTypeName(token.type) << " (value = " << token.getValue<double>() << ")";
            break;
        case Scanner::Token::Type::Identifier:
            out << Scanner::Token::getTypeName(token.type);
            if (token.value.length() > Scanner::Token::identifierMaxLength)
                out << "(truncated to " + token.getValue<std::string>() + ")";
            break;

        default:
            out << Scanner::Token::getTypeName(token.type);
        // ommitting default case here since we may add other token types with different
        // print options
    };

    out << std::endl;
    // not ending with new line since caller may want to print their own new line

    return out;
}

