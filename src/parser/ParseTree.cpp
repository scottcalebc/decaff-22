
#include <string>
#include <sstream>
#include <iomanip>
#include "ParseTree.hpp"


std::string DeclarationType::toString(int numSpaces)
{
    std::stringstream ss;
    // we don't print number line for type
    ss  << "Type: " << type.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string VariableDeclaration::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << "VarDecl:" << std::endl
        << std::setw(numSpaces + 3) << " " 
        << type->toString(numSpaces + 3)
        << std::setw(3) << ident.lineNumber
        << std::setw(numSpaces) << " "
        << "Identifier: " << ident.getValue<std::string>() 
        << std::endl;

    return ss.str();
}

std::string FormalVariableDeclaration::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << "(formals) " << VariableDeclaration::toString(numSpaces);

    return ss.str();
}


std::string FunctionDeclaration::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << "FnDecl:" << std::endl
        << std::setw(numSpaces + 3) << " " 
        << "(return type) " << type->toString(numSpaces + 3)
        << std::setw(3) << ident.lineNumber
        << std::setw(numSpaces) << " "
        << "Identifier: " << ident.getValue<std::string>() 
        << std::endl;

    for (auto formal : formals)
    {
        ss  << std::setw(3) << ident.lineNumber
            << std::setw(numSpaces) << " " 
            << formal->toString(numSpaces + 3);
    }

    return ss.str();
}