

#include "generate.hpp"
#include "SymbolTableVisitor.hpp"


void SymbolTable::generate(AST::Program *p)
{
    SymbolTable::STVisitor visitor = SymbolTable::STVisitor();

    p->accept(&visitor);

}