#include <exception>
#include <iomanip>

#include "generate.hpp"
#include "SymbolTableVisitor.hpp"


SymbolTable::Exception::Exception(char *msg)
    : message(msg)
{

}

SymbolTable::Exception::Exception(std::string message, std::string id)
{
    std::stringstream ss;
    ss << std::endl
        << "*** Error: " << message << " : " << id << std::endl;

    message = ss.str();
}

SymbolTable::Exception::Exception(int start, int length, int lineNumber, std::string line, std::string msg)
{
    std::stringstream ss;
    ss << std::endl
        << "*** Error line " << lineNumber << ".\n"
        << line << std::endl
        << std::setw(start-1) << " "
        << std::setfill('^') << std::setw(length) << "" << std::endl
        << "*** " << msg << std::endl;

    message = ss.str();
}

const char * SymbolTable::Exception::what()
{
    return message.c_str();
}

SymbolTable::IdEntry *SymbolTable::Scope::install(std::string id, Scanner::Token::Type type, int block)
{
    auto e = new IdEntry(id, type, block);

    TableIterator it ( table.find(id) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw Exception("Cannot redeclare variable in same scope", id);
        // throw std::runtime_error("Cannot redeclare variable in same scope");

    table.insert( {id, e} );
    return e;
}

SymbolTable::IdEntry *SymbolTable::Scope::install(AST::Declaration* id, int block)
{
    auto e = new IdEntry(id->ident.getValue<std::string>() , id->type, block);
    TableIterator it ( table.find(id->ident.getValue<std::string>()) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw Exception(id->ident.colStart, id->ident.value.length(), id->ident.lineNumber, id->ident.lineInfo, "Cannot redeclare variable in same scope");
        // throw std::runtime_error("Cannot redeclare variable in same scope");
    table.insert( {id->ident.value, e} );
    return e;
}

SymbolTable::IdEntry *SymbolTable::Scope::install(AST::FunctionDeclaration* id, int block)
{
    auto e = new IdEntry(id->ident.getValue<std::string>() , id->type, block, true);
    TableIterator it ( table.find(id->ident.getValue<std::string>()) );

    // Handle name collisions within a single scope
    if (it != table.end())
        throw Exception(id->ident.colStart, id->ident.value.length(), id->ident.lineNumber, id->ident.lineInfo, "Cannot redeclare variable in same scope");
        // throw std::runtime_error("Cannot redeclare variable in same scope");
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

int SymbolTable::Scope::getNextParamOffset()
{
    if (parentScope != nullptr && parentScope->parentScope == nullptr)
    {
        paramOffset += 4;
        return paramOffset;
    }

    // we are in global scope so return global scopes offset
    if (parentScope == nullptr)
    {
        paramOffset += 4;
        return paramOffset;
    }
    
    return parentScope->getNextOffset();
}

int SymbolTable::Scope::getNextOffset()
{
    // if our parent has null parent, then parent is global and we are in 
    //  functio scope which holds base offset
    if (parentScope != nullptr && parentScope->parentScope == nullptr)
    {
        baseOffset += 4;
        return baseOffset;
    }

    // we are in global scope so return global scopes offset
    if (parentScope == nullptr)
    {
        baseOffset += 4;
        return baseOffset;
    }
    
    return parentScope->getNextOffset();
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