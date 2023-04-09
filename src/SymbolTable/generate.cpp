

#include "generate.hpp"
#include "SymbolTableVisitor.hpp"


SymbolTable::IdEntry SymbolTable::Scope::install(std::string id, Scanner::Token::Type type, int block)
{
    return IdEntry(id, type, block);
}

SymbolTable::IdEntry SymbolTable::Scope::install(AST::Declaration* id, int block)
{
    return IdEntry(id->ident.getValue<std::string>() , id->type, block);
}

// SymbolTable::IdEntry SymbolTable::Scope::idLookup(std::string id, int block)
// {
    

//     return IdEntry(id, block);
// }

void SymbolTable::generate(AST::Program *p)
{
    SymbolTable::STVisitor visitor = SymbolTable::STVisitor();

    p->accept(&visitor);

}