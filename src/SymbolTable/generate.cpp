#include <exception>
#include <iomanip>

#include "generate.hpp"
#include "SymbolTableVisitor.hpp"

SymbolTable::IdEntry *SymbolTable::Scope::install(std::string id, Scanner::Token::Type type, int block)
{
    auto e = new IdEntry(id, type, block);

    TableIterator it ( table.find(id) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw std::runtime_error("Cannot redeclare variable in same scope");

    table.insert( {id, e} );
    return e;
}

SymbolTable::IdEntry *SymbolTable::Scope::install(AST::Declaration* id, int block)
{
    auto e = new IdEntry(id->ident.getValue<std::string>() , id->type, block);
    TableIterator it ( table.find(id->ident.getValue<std::string>()) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw std::runtime_error("Cannot redeclare variable in same scope");
    table.insert( {id->ident.value, e} );
    return e;
}

SymbolTable::IdEntry *SymbolTable::Scope::install(AST::FunctionDeclaration* id, int block)
{
    auto e = new IdEntry(id->ident.getValue<std::string>() , id->type, block, true);
    TableIterator it ( table.find(id->ident.getValue<std::string>()) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw std::runtime_error("Cannot redeclare variable in same scope");
    table.insert( {id->ident.value, e} );
    return e;
}

SymbolTable::IdEntry *SymbolTable::Scope::fakeInstall(std::string str)
{
    auto e = new IdEntry(str);
    table.insert( {str, e});

    return e;
}


SymbolTable::IdEntry* SymbolTable::Scope::idLookup(std::string id)
{
    TableIterator it ( table.find(id) );

    if ( it == table.end() )
    {
        if (parentScope != nullptr)
        {
            return parentScope->idLookup(id);
        }
        else {
            // throw std::runtime_error("No symbol with id: " + id);
            return nullptr;
        }
    }
    
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

Scanner::Token::Type SymbolTable::Scope::getReturnType()
{
    if (returnType != Scanner::Token::Type::ERROR)
    {
        return returnType;
    }
    
    if (parentScope != nullptr)
        return parentScope->getReturnType();

    return returnType;
}

SymbolTable::Scope * SymbolTable::Scope::funcLookup(SymbolTable::IdEntry* entry)
{
    if (parentScope != nullptr)
        return parentScope->funcLookup(entry);

    std::map<std::string, SymbolTable::Scope*>::iterator it ( funcScope.find(entry->ident) );

    if ( it == funcScope.end() )
        throw std::runtime_error( "No Scope for function " + entry->ident );

    return it->second;

}



void SymbolTable::generate(AST::Program *p)
{
    SymbolTable::STVisitor visitor = SymbolTable::STVisitor();

    p->accept(&visitor);

}