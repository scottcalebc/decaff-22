
#include <string>
#include <sstream>
#include <iomanip>
#include "ParseTree.hpp"


std::string DeclarationType::toString(int numSpaces)
{
    std::stringstream ss;
    ss  << std::setw(numSpaces) << " "
        << "Type: " << type.getValue<std::string>() << std::endl;

    return ss.str();
}


std::string VariableDeclaration::toString(int numSpaces)
{
    std::stringstream ss;

    ss  << std::setw(numSpaces) << " "
        << "VarDecl:" << std::endl
        << type->toString(numSpaces + 3)
        << std::setw(numSpaces + 3) << " "
        << "Identifier: " << ident.getValue<std::string>() << std::endl;

    return ss.str();
}