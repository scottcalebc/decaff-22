#include "ParseTreeVisitor.hpp"
#include "parser/exceptions.hpp"


void AST::ParseTreeConverter::convert(Parser::Identifier *p)
{
    pNode = new AST::Ident(p->ident);
}

void AST::ParseTreeConverter::convert(Parser::DeclarationType *p)
{
    std::cout << "Declaration type hit, not sure this should happen\n";
}

void AST::ParseTreeConverter::convert(Parser::ReturnType *p)
{
    std::cout << "Return type hit\n";
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
    pNode = new AST::Constant(p->constant);
}

void AST::ParseTreeConverter::convert(Parser::UnaryExpression *p)
{
    switch(p->op.subType)
    {
        case Scanner::Token::SubType::UnaryNegative:
            pNode = new Subtract(p);
            break;
        case Scanner::Token::SubType::Not:
            pNode = new Not(p);
            break;
        default:
            break;
    }

    if (pNode == nullptr)
        throw Parser::ParseException( p->firstToken() );

}

void AST::ParseTreeConverter::convert(Parser::CallExpression *p)
{
    pNode = new Call(p);
}

void AST::ParseTreeConverter::convert(Parser::ParenExpr *p)
{
    p->expr->accept(this);
}

void AST::ParseTreeConverter::convert(Parser::PrintStmt *p)
{
    pNode = new Print(p);
}

void AST::ParseTreeConverter::convert(Parser::ReadIntExpr *p)
{
    pNode = new ReadInteger();
}

void AST::ParseTreeConverter::convert(Parser::ReadLineExpr *p)
{
    pNode = new ReadLine();
}

void AST::ParseTreeConverter::convert(Parser::RelationalExpression *p)
{
    switch(p->op.subType)
    {
        case Scanner::Token::SubType::LessThan:
            pNode = new LessThan(p);
            break;
        case Scanner::Token::SubType::LessEqual:
            pNode = new LTE(p);
            break;
        case Scanner::Token::SubType::GreaterThan:
            pNode = new GreaterThan(p);
            break;
        case Scanner::Token::SubType::GreaterEqual:
            pNode = new GTE(p);
            break;
        default:
            break;
    }

    if (pNode == nullptr)
        throw Parser::ParseException( p->firstToken() );
}

void AST::ParseTreeConverter::convert(Parser::EqualityExpression *p)
{
    switch(p->op.subType)
    {
        case Scanner::Token::SubType::Equal :
            pNode = new Equal(p);
            break;
        case Scanner::Token::SubType::NotEqual :
            pNode = new NotEqual(p);
            break;
        default:
            break;
    }
    
    if (pNode == nullptr)
        throw Parser::ParseException( p->firstToken() );
}

void AST::ParseTreeConverter::convert(Parser::LogicalExpression *p)
{
    switch (p->op.subType)
    {
        case Scanner::Token::SubType::And :
            pNode = new And(p);
            break;
        case Scanner::Token::SubType::Or:
            pNode = new Or(p);
            break;    
        default:
            break;
    }

    if (pNode == nullptr)
        throw Parser::ParseException( p->firstToken() );
}

void AST::ParseTreeConverter::convert(Parser::ArithmeticExpression *p)
{
    switch(p->op.subType)
    {
        case Scanner::Token::SubType::Add:
            pNode = new Add(p);
            break;
        case Scanner::Token::SubType::Subtract:
            pNode = new Subtract(p);
            break;
        case Scanner::Token::SubType::Divide:
            pNode = new Divide(p);
            break;
        case Scanner::Token::SubType::Multiply:
            pNode = new Multiply(p);
            break;
        case Scanner::Token::SubType::Modulus:
            pNode = new Modulus(p);
            break;
        default:
            std::cout << "Unexpected token: " << p->op.getValue<std::string>() << std::endl;
    }

    if (pNode == nullptr)
        throw Parser::ParseException( p->firstToken() );
}

void AST::ParseTreeConverter::convert(Parser::AssignExpression *p)
{
    pNode = new Assign(p);
}

void AST::ParseTreeConverter::convert(Parser::BreakStmt *p)
{
    pNode = new Break();
}

void AST::ParseTreeConverter::convert(Parser::WhileStmt *p)
{
    pNode = new While(p);
}

void AST::ParseTreeConverter::convert(Parser::IfStmt *p)
{
    pNode = new If(p);
}

void AST::ParseTreeConverter::convert(Parser::ForStmt *p)
{
    pNode = new For(p);
}

void AST::ParseTreeConverter::convert(Parser::StatementBlock *p)
{
    pNode = new AST::StatementBlock(p);
}

void AST::ParseTreeConverter::convert(Parser::FormalVariableDeclaration *p)
{
    pNode = new Declaration(p);
}

void AST::ParseTreeConverter::convert(Parser::VariableDeclaration *p)
{
    pNode = new Declaration(p);
}

void AST::ParseTreeConverter::convert(Parser::FunctionDeclaration *p)
{
    pNode = new AST::FunctionDeclaration(p);
}

void AST::ParseTreeConverter::convert(Parser::Program *p)
{
    pNode = new AST::Program(p);
}