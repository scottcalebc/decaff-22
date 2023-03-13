
#include <string>
#include <sstream>
#include <iomanip>
#include "ParseTree.hpp"

FormalVariableDeclaration::FormalVariableDeclaration(VariableDeclaration *var)
{
    if (var == nullptr)
        throw std::runtime_error("Invalid Formal");
    
    // var = var;
    ident = new Identifier();
    *ident = *var->ident;
    type = new DeclarationType();
    *type = *var->type;

}


std::string Identifier::toString(int numSpaces)
{
    std::stringstream ss;

    ss << nodeName() << ident.getValue<std::string>();

    return ss.str();
}

std::string DeclarationType::toString(int numSpaces)
{
    std::stringstream ss;
    // we don't print number line for type
    ss << nodeName() << type.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string Declarations::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << this->nodeName() << std::endl
        << std::setw(numSpaces + 3) << " " 
        << type->toString(numSpaces + 3)
        << std::setw(3) << ident->line()
        << std::setw(numSpaces) << " "
        << ident->toString(numSpaces + 3)
        << std::endl;

    return ss.str();
}

std::string LValue::toString(int numSpaces)
{
    std::stringstream ss;

    ss << "FieldAccess:" << std::endl
        << std::setw(3) << ident->line()
        << std::setw(numSpaces) << " "
        << ident->toString(numSpaces+3);

    return ss.str();
}

std::string Constant::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << Scanner::Token::getTypeName(constant.type) 
        << ": " << constant.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string ParenExpr::toString(int numSpaces)
{
    return expr->toString(numSpaces);
}


std::string Statement::toString(int numSpaces)
{
    std::stringstream ss;

    ss << nodeName() << std::endl;

    return ss.str();
}

std::string StatementBlock::toString(int numSpaces)
{
    std::stringstream ss;

    ss << Statement::toString(numSpaces);

    for(auto var : vars)
    {
        ss << std::setw(3) << var->line();
        ss << std::setw(numSpaces) << " "; 
        ss << var->toString(numSpaces+3);
    }

    return ss.str();
}

std::string FunctionDeclaration::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << Declarations::toString(numSpaces);

    for (auto formal : formals)
    {
        ss  << std::setw(3) << ident->line()
            << std::setw(numSpaces) << " " 
            << formal->toString(numSpaces + 3);
    }

    ss << std::setw(numSpaces + 3) << " ";
    ss << "(body) ";
    ss << block->toString(numSpaces+3);

    return ss.str();
}

