#include <iostream>

#include <parser/exceptions.hpp>

#include "AbstractSyntaxTree.hpp"
#include "ParseTreeVisitor.hpp"



namespace AST
{

    KeywordStmt::KeywordStmt(Parser::KeywordStmt *stmt)
        : Value(stmt->keyword)
        , expr(nullptr)
    {
        ParseTreeConverter visitor = ParseTreeConverter();

        stmt->expr->accept(&visitor);
        expr = visitor.pNode;

        if (visitor.pNode == nullptr)
            throw Parser::ParseException( stmt->firstToken() );
    }

    While::While(Parser::WhileStmt *stmt)
        : KeywordStmt(stmt)
    {
        ParseTreeConverter visitor = ParseTreeConverter();

        stmt->stmt->accept(&visitor);
        expr = visitor.pNode;

        if (visitor.pNode == nullptr)
            throw Parser::ParseException( stmt->firstToken() );
    };

    If::If(Parser::IfStmt *stmt)
        : While(stmt)
    {
        if ( stmt->elseBlock != nullptr )
        {
            ParseTreeConverter visitor = ParseTreeConverter();

            stmt->stmt->accept(&visitor);
            elseStmt = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( stmt->firstToken() );
        }
    }

    For::For(Parser::ForStmt *stmt)
        : While(stmt)
    {
        if (stmt->startExpr != nullptr)
        {
            std::cout << "For: getting start expr\n";
            ParseTreeConverter visitor = ParseTreeConverter();

            stmt->startExpr->accept(&visitor);
            startExpr = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( stmt->firstToken() );
        }

        if (stmt->loopExpr != nullptr)
        {
            std::cout << "For: getting loop expr\n";
            ParseTreeConverter visitor = ParseTreeConverter();

            stmt->loopExpr->accept(&visitor);
            loopExpr = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( stmt->firstToken() );
        }
    }

    Call::Call(Parser::CallExpression *c)
        : Value(c->ident->ident)
    {
        std::cout << "Call: Generating call with identifier " << value.getValue<std::string>() << std::endl;

        for ( auto &var : c->actuals )
        {
            ParseTreeConverter visitor = ParseTreeConverter();

            var->accept(&visitor);
            actuals.push_back(visitor.pNode);

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( c->firstToken() );
        }
    }

    Expr::Expr(Parser::Expression *expr)
    {
        if (expr->expr != nullptr )
        {
            ParseTreeConverter visitor = ParseTreeConverter();
            expr->expr->accept(&visitor);
            left = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->firstToken() );
        }
    }

    Expr::Expr(Parser::BinaryExpression *expr)
    {
        op = expr->op;
        
        ParseTreeConverter visitor = ParseTreeConverter();
        if (expr->expr != nullptr )
        {
            expr->expr->accept(&visitor);
            left = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->expr->firstToken() );
            visitor.pNode = nullptr;
        }

        if ( expr->right != nullptr )
        {
            expr->right->accept(&visitor);
            right = visitor.pNode;
            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->right->firstToken() );
            visitor.pNode = nullptr;
        }
    }

    Return::Return(Parser::ReturnStmt *stmt)
    {
        
        if ( stmt->expr != nullptr )
        {
            std::cout << "Return: Getting expr\n";
            // expr = new Expr
            ParseTreeConverter visitor = ParseTreeConverter();

            stmt->expr->accept(&visitor);
            Node *pNode(visitor.pNode);

            if ( pNode == nullptr )
                throw Parser::ParseException( stmt->firstToken() );
        }
        else
            std::cout << "Return: \n";
    }

    StatementBlock::StatementBlock(Parser::StatementBlock *block)
        : Node()
        , decls()
        , stmts()
    {
        for (auto &decl : block->vars)
        {
            decls.push_back(new Declaration(decl));
        }

        int vars( decls.size() );

        for( auto &stmt: block->stmts )
        {
            ParseTreeConverter visitor = ParseTreeConverter();
            // visit node and covnert
            stmt->accept(&visitor);
            Node *pNode( visitor.pNode );

            if (pNode != nullptr)
            {
                stmts.push_back(pNode);
            }
            else
                throw Parser::ParseException( stmt->firstToken() );
        }

    }

    FunctionDeclaration::FunctionDeclaration(Parser::FunctionDeclaration *func)
                : Declaration(func)
                , formals()
                , stmts(nullptr)
    {
        for( auto &decl : func->formals)
        {
            formals.push_back(new Declaration(decl));
        }
        stmts = new StatementBlock(func->block);
    };

    Program::Program(Parser::Program *p)
        : Node()
        , decls()
    {
        for(auto &decl : p->decls)
        {
            ParseTreeConverter converter = ParseTreeConverter();
            decl->accept(&converter);
            decls.push_back(converter.pNode);

            if (converter.pNode == nullptr)
                throw Parser::ParseException( decl->firstToken() );

        }

        return;
    }
} // namespace AST
