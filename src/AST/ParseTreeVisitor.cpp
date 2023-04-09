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

void AST::ParseTreeConverter::convert(Parser::ReturnStmt *p)
{
    std::cout << "Return hit\n";
    pNode = new AST::Return(p);
}

void AST::ParseTreeConverter::convert(Parser::LValue *p)
{
    std::cout << "LValue hit\n";
    pNode = new AST::Ident(p->ident->ident);
}
void AST::ParseTreeConverter::convert(Parser::Constant *p)
{
    std::cout << "Constant value hit\n";
    pNode = new AST::Ident(p->constant);
}

void AST::ParseTreeConverter::convert(Parser::ReturnType *p)
{
    std::cout << "Return type hit\n";
}

void AST::ParseTreeConverter::convert(Parser::UnaryExpression *p)
{
    std::cout << "Unary Expr hit\n";

    if (p->op.getValue<std::string>().compare("-") == 0)
    {
        pNode = new Subtract(p);
    }
    else if ( p->op.getValue<std::string>().compare("!") == 0 )
    {
        pNode = new Not(p);
    }
}

void AST::ParseTreeConverter::convert(Parser::CallExpression *p)
{
    std::cout << "Call Expr hit\n";
    pNode = new Call(p);
}

void AST::ParseTreeConverter::convert(Parser::ParenExpr *p)
{
    std::cout << "Paren Expr hit\n";
    p->expr->accept(this);
}

void AST::ParseTreeConverter::convert(Parser::PrintStmt *p)
{
    std::cout << "Print Stmt hit\n";
    pNode = new Print(p);
}

void AST::ParseTreeConverter::convert(Parser::ReadIntExpr *p)
{
    std::cout << "Read Int hit\n";
    pNode = new ReadInteger();
}

void AST::ParseTreeConverter::convert(Parser::ReadLineExpr *p)
{
    std::cout << "Read line hit\n";
    pNode = new ReadLine();
}

void AST::ParseTreeConverter::convert(Parser::RelationalExpression *p)
{
    std::cout << "Relation Expr hit\n";
}

void AST::ParseTreeConverter::convert(Parser::EqualityExpression *p)
{
    std::cout << "Equality Expr hit\n";

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
    std::cout << "Logical Expr hit\n";
    
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
    std::cout << "Arithmetic Expr hit\n";

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
    std::cout << "Assign Expr hit\n";
    pNode = new Assign(p);
}

void AST::ParseTreeConverter::convert(Parser::BreakStmt *p)
{
    std::cout << "Break stmt hit\n";
}

void AST::ParseTreeConverter::convert(Parser::WhileStmt *p)
{
    std::cout << "While Stmt hit\n";
}

void AST::ParseTreeConverter::convert(Parser::IfStmt *p)
{
    std::cout << "If stmt hit\n";
}

void AST::ParseTreeConverter::convert(Parser::ForStmt *p)
{
    std::cout << "For Stmt hit\n";
}

void AST::ParseTreeConverter::convert(Parser::StatementBlock *p)
{
    pNode = new AST::StatementBlock(p);
}

void AST::ParseTreeConverter::convert(Parser::FormalVariableDeclaration *p)
{
    std::cout << "Formal Declaration type hit\n";
}

void AST::ParseTreeConverter::convert(Parser::VariableDeclaration *p)
{
    std::cout << "Variable Declaration type hit\n";
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