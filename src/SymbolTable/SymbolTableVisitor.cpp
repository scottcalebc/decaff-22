
#include <AST/AbstractSyntaxTree.hpp>
#include "SymbolTableVisitor.hpp"




void SymbolTable::STVisitor::visit(AST::FunctionDeclaration *p)
{
    
    std::cout << "Visiting function: " << p->ident.getValue<std::string>() << std::endl;

    Scope *parentScope = currScope;
    currScope = new Scope();
    currScope->parentScope = parentScope;   // this will be useful for reverse lookups

    for ( auto &param : p->formals )
    {
        currScope->install(param, 2);
    }

    // add function decls to scope
    for (auto &decl : p->stmts->decls)
    {
        currScope->install(decl, 3);
    }

    // visit statements for any that have their own scope (i.e. StatementBlock, If, etc.)
    for (auto &stmt : p->stmts->stmts )
    {
        stmt->accept(this);
    }

    // reset scope
    currScope = parentScope;
}


void SymbolTable::STVisitor::visit(AST::Program *p)
{
    std::cout << "Visiting program \n";

    currScope = new Scope();

    for (auto &node : p->decls)
    {
        if (dynamic_cast<AST::FunctionDeclaration*>(node) != nullptr )
        {
            // Add function entry to global table
            currScope->install(dynamic_cast<AST::Declaration*>(node), 1);

            // pass visitor to function 
            node->accept(this);
        }
        else if (dynamic_cast<AST::Declaration*>(node) != nullptr )
        {
            // insert into symbol table as global
            currScope->install(dynamic_cast<AST::Declaration*>(node), 1);
        }
    }
}