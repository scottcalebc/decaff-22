
#include <string>
#include <sstream>
#include <iomanip>
#include "ParseTree.hpp"


std::string Identifier::nodeName()
{
    return "Identifier: ";
}

std::string Identifier::toString(int numSpaces)
{
    std::stringstream ss;

    ss << nodeName() << ident.getValue<std::string>();

    return ss.str();
}


std::string ReturnType::nodeName()
{
    return "(return type) " + DeclarationType::nodeName();
}

std::string DeclarationType::nodeName()
{
    return "Type: ";
}

std::string DeclarationType::toString(int numSpaces)
{
    std::stringstream ss;
    // we don't print number line for type
    ss << nodeName() << type.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string LValue::toString(int numSpaces)
{
    std::stringstream ss;

    ss << "Reached LValue" << std::endl;

    return ss.str();
}


std::string VariableDeclaration::nodeName()
{
    return "VarDecl:";
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
std::string FormalVariableDeclaration::nodeName()
{
    return "(formals) " + VariableDeclaration::nodeName();
}


std::string FunctionDeclaration::nodeName()
{
    return "FnDecl:";
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

    return ss.str();
}