#include <exception>
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

SymbolTable::IdEntry SymbolTable::Scope::install(AST::FunctionDeclaration* id, int block)
{
    return IdEntry(id->ident.getValue<std::string>() , id->type, block, true);
}


SymbolTable::IdEntry SymbolTable::Scope::idLookup(std::string id)
{
    std::map<std::string, IdEntry>::iterator it ( table.find(id) );

    if ( it == table.end() )
        throw std::runtime_error("No symbol with id: " + id);

    return it->second;
}



void SymbolTable::generate(AST::Program *p)
{
    SymbolTable::STVisitor visitor = SymbolTable::STVisitor();

    p->accept(&visitor);

}