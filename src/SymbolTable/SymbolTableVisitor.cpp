
#include <AST/AbstractSyntaxTree.hpp>
#include "SymbolTableVisitor.hpp"


void SymbolTable::STVisitor::visit(AST::Program *p)
{
    std::cout << "Visiting prog \n";
}