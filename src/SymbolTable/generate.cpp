#include <exception>
#include <iomanip>

#include "generate.hpp"
#include "SymbolTableVisitor.hpp"

SymbolTable::IdEntry SymbolTable::Scope::install(std::string id, Scanner::Token::Type type, int block)
{
    auto e = IdEntry(id, type, block);
    table.insert( {id, e} );
    return e;
}

SymbolTable::IdEntry SymbolTable::Scope::install(AST::Declaration* id, int block)
{
    auto e = IdEntry(id->ident.getValue<std::string>() , id->type, block);
    table.insert( {id->ident.value, e} );
    return e;
}

SymbolTable::IdEntry SymbolTable::Scope::install(AST::FunctionDeclaration* id, int block)
{
    auto e = IdEntry(id->ident.getValue<std::string>() , id->type, block, true);
    table.insert( {id->ident.value, e} );
    return e;
}


SymbolTable::IdEntry SymbolTable::Scope::idLookup(std::string id)
{
    std::map<std::string, IdEntry>::iterator it ( table.find(id) );

    if ( it == table.end() )
        throw std::runtime_error("No symbol with id: " + id);

    return it->second;
}

std::string SymbolTable::Scope::toString(int &space)
{
    std::stringstream ss;

    if (parentScope != nullptr)
        ss << parentScope->toString(space);

    ss << std::setw(space) << " " << "Scope:\n";
    for ( const auto &pair : table ) {
        ss << std::setw(space+3) << " " << pair.first << "\n";
    }

    space += 3;

    return ss.str();

}



void SymbolTable::generate(AST::Program *p)
{
    SymbolTable::STVisitor visitor = SymbolTable::STVisitor();

    p->accept(&visitor);

}