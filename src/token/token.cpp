#include <iomanip>
#include "token.hpp"


std::map<Token::Type, std::string> Token::enumName{
    {Type::IntConstant, "IntConstant"},
    {Type::DoubleConstant, "DoubleConstant"},
    {Type::StringConstant, "StringConstant"},
    {Type::Identifier, "Identifier"}
};

std::ostream& operator<<(std::ostream &out, Token const& token)
{

    // print token first with set spacing
    out << token.value << std::setw(20)
        << "line " << token.lineNumber << " cols " << token.colStart << "-"
        << token.colStart + token.value.length() << " is " ;

    switch (token.type) {
        case Token::Type::Separator:
        case Token::Type::Operator:
            out << token.value;
            break;
        case Token::Type::IntConstant:
        case Token::Type::DoubleConstant:
        case Token::Type::StringConstant:
            out << Token::getTypeName(token.type) << " (value = " << token.value << ")";
            break;
        default:
            out << Token::getTypeName(token.type);
        // ommitting default case here since we may add other token types with different
        // print options
    };

    // not ending with new line since caller may want to print their own new line

    return out;
}

