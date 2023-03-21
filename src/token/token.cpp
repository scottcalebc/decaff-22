#include <iomanip>
#include "token.hpp"


namespace Scanner{
std::map<Scanner::Token::Type, std::string> Scanner::Token::enumName{
    {Type::Or, "Or"},
    {Type::If, "If"},
    {Type::Int, "Int"},
    {Type::For, "For"},
    {Type::And, "And"},
    {Type::Else, "Else"},
    {Type::Void, "Void"},
    {Type::Bool, "Bool"},
    {Type::Break, "Break"},
    {Type::While, "While"},
    {Type::Equal, "Equal"},
    {Type::String, "String"},
    {Type::Double, "Double"},
    {Type::Return, "Return"},
    {Type::NotEqual, "NotEqual"},
    {Type::LessEqual, "LessEqual"},
    {Type::Identifier, "Identifier"},
    {Type::IntConstant, "IntConstant"},
    {Type::BoolConstant, "BoolConstant"},
    {Type::GreaterEqual, "GreaterEqual"},
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

std::map<std::string, Scanner::Token::SubType> Scanner::Token::subTypes{
    {"(", SubType::Paren},
    {")", SubType::Paren},
    {",", SubType::Comma},
    {"=", SubType::Assign},
    {"!", SubType::Not},
    {"+", SubType::Add},
    {"-", SubType::Subtract},
    {"*", SubType::Multiply},
    {"/", SubType::Divide},
    {"%", SubType::Modulus},
    {"<", SubType::LessThan},
    {"<=", SubType::LessEqual},
    {">", SubType::GreaterThan},
    {">=", SubType::GreaterEqual},
    {"==", SubType::Equal},
    {"!=", SubType::NotEqual},
    {"&&", SubType::And},
    {"||", SubType::Or},
};

int Scanner::Token::identifierMaxLength = 31;

template<>
const std::string Scanner::Token::getValue() const
{
    return (type == Type::Identifier) ?
        value.substr(0, Token::identifierMaxLength) :
        value;
}

template<>
const double Scanner::Token::getValue() const
{
    return std::atof(value.c_str());
}

template<>
const int Scanner::Token::getValue() const
{
    return std::atoi(value.c_str());
}

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
            out << "\'" << token.getValue<std::string>()  << "\'";
            break;
        case Scanner::Token::Type::BoolConstant:
        case Scanner::Token::Type::StringConstant:
            out << Scanner::Token::getTTypeName(token.type) << " (value = " << token.getValue<std::string>() << ")";
            break;
        case Scanner::Token::Type::IntConstant:
            out << Scanner::Token::getTTypeName(token.type) << " (value = " << token.getValue<int>() << ")";
            break;
        case Scanner::Token::Type::DoubleConstant:
            out << Scanner::Token::getTTypeName(token.type) << " (value = " << token.getValue<double>() << ")";
            break;
        case Scanner::Token::Type::Identifier:
            out << Scanner::Token::getTTypeName(token.type);
            if (token.value.length() > Scanner::Token::identifierMaxLength)
                out << "(truncated to " + token.getValue<std::string>() + ")";
            break;

        default:
            out << Scanner::Token::getTTypeName(token.type);
        // ommitting default case here since we may add other token types with different
        // print options
    };

    out << std::endl;
    // not ending with new line since caller may want to print their own new line

    return out;
}

