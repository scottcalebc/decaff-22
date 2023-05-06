
#include <AST/AbstractSyntaxTree.hpp>
#include "SymbolTableVisitor.hpp"
#include "Entities.hpp"


void SymbolTable::STVisitor::visit(AST::KeywordStmt *p)
{
    p->setScope( currScope );
}

void SymbolTable::STVisitor::visit(AST::While *p)
{
    p->setScope( currScope );
    p->stmt->accept( this );
}

void SymbolTable::STVisitor::visit(AST::For *p)
{
    p->setScope( currScope );
    p->stmt->accept( this );
}

void SymbolTable::STVisitor::visit(AST::If *p)
{
    p->setScope( currScope );

    if (p->stmt != nullptr)
        p->stmt->accept( this );

    if (p->elseStmt != nullptr)
        p->elseStmt->accept( this );
}


void SymbolTable::STVisitor::visit(AST::StatementBlock *p)
{
    Scope *parentScope = currScope;
    currScope = new Scope();
    currScope->parentScope = parentScope;
    p->setScope( currScope );

    for ( auto &var : p->decls )
    {
        currScope->install(var, 3);
    }

    for ( auto &stmt : p->stmts )
    {
        stmt->accept( this );
    }

}

void SymbolTable::STVisitor::visit(AST::FunctionDeclaration *p)
{
    Scope *parentScope = currScope;
    currScope = new Scope();
    currScope->parentScope = parentScope;   // this will be useful for reverse lookups 
    currScope->returnType = p->type;
    p->setScope( currScope );

    int idx = 0;
    for ( auto &param : p->formals )
    {
        // index should be parameter index, this will be used during type checking
        IdEntry *id = currScope->install(param, 2);
        id->paramIndex = idx++;
    }

    currScope->numOfParams = p->formals.size();


    // Statement block has it's own scope, this allows shadowing of parameters
    p->stmts->accept(this);


    // reset scope
    currScope = parentScope;
}


void SymbolTable::STVisitor::visit(AST::Program *p)
{
    currScope = new Scope();
    p->setScope( currScope );

    // install functions
    // currScope->install("ReadLine", Scanner::Token::Type::Identifier, 1);
    // currScope->install("ReadInteger", Scanner::Token::Type::Identifier, 1);
    // currScope->install("Print", Scanner::Token::Type::Identifier, 1);

    for ( auto &node : p->vars )
    {
        // insert into symbol table as global
        currScope->install(dynamic_cast<AST::Declaration*>(node), 1);
    }

    for ( auto &node : p->func )
    {
        SymbolTable::IdEntry *e = currScope->install(dynamic_cast<AST::Declaration*>(node), 1);
        e->func = true;
        node->accept(this);

        currScope->funcScope.insert( { e->ident, node->pScope });
    }

}