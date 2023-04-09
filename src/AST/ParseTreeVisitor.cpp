#include "ParseTreeVisitor.hpp"


void AST::ParseTreeConverter::convert(Parser::Identifier *p)
{
    pNode = new AST::Ident(p->ident);
}

void AST::ParseTreeConverter::convert(Parser::ReturnStmt *p)
{
    pNode = new AST::Return(p);
}

void AST::ParseTreeConverter::convert(Parser::LValue *p)
{
    pNode = new AST::Ident(p->ident->ident);
}
void AST::ParseTreeConverter::convert(Parser::Constant *p)
{
    pNode = new AST::Ident(p->constant);
}

void AST::ParseTreeConverter::convert(Parser::StatementBlock *p)
{
    pNode = new AST::StatementBlock(p);
}

void AST::ParseTreeConverter::convert(Parser::FunctionDeclaration *p)
{
    std::cout << "Visiting Function Decl\n";
    pNode = new AST::FunctionDeclaration(p);
}

void AST::ParseTreeConverter::convert(Parser::Program *p)
{
    pNode = new AST::Program(p);
}