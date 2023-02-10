#include <iomanip>
#include "token.hpp"

std::map<Token::Type, std::string> Token::enumName{
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

std::map<std::string, Token::Type> Token::keywords{
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

std::map<std::string, Token::Type> Token::operators{
    {"!=", Type::NotEqual},
    {"==", Type::Equal},
    {">=", Type::GreaterEqual},
    {"<=", Type::LessEqual},
    {"||", Type::Or},
    {"&&", Type::And},
};

int Token::identifierMaxLength = 32;

std::ostream& operator<<(std::ostream &out, Token const& token)
{
    // if end token don't print anything
    if (token.type == Token::Type::END)
        return out;

    // print token first with set spacing
    out << token.value << std::setw(20)
        << "line " << token.lineNumber << " cols " << token.colStart << "-"
        << (token.colStart + token.value.length()) - 1 << " is " ;

    switch (token.type) {
        case Token::Type::Separator:
        case Token::Type::Operator:
            out << "\'" << token.value << "\'";
            break;
        case Token::Type::IntConstant:
        case Token::Type::DoubleConstant:
        case Token::Type::StringConstant:
            out << Token::getTypeName(token.type) << " (value = " << token.value << ")";
            break;

        case Token::Type::Identifier:
            out << Token::getTypeName(token.type);
            if (token.value.length() > Token::identifierMaxLength)
                out << "(truncated to " + token.getValue() + ")";

            break;

        default:
            out << Token::getTypeName(token.type);
        // ommitting default case here since we may add other token types with different
        // print options
    };

    out << std::endl;
    // not ending with new line since caller may want to print their own new line

    return out;
}

const std::string Token::getValue() const
{
    return (type == Type::Identifier) 
        ? value.substr(0, Token::identifierMaxLength+1)
        : value;
}

