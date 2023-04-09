#include <iostream>

#include <parser/exceptions.hpp>

#include "AbstractSyntaxTree.hpp"
#include "ParseTreeVisitor.hpp"



namespace AST
{

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
        std::cout << "Expr: Generating abstract expr\n";
        
        if (expr->expr != nullptr )
        {
            ParseTreeConverter visitor = ParseTreeConverter();

            std::cout << "Expr: Visiting LHS expr\n";
            expr->expr->accept(&visitor);
            left = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->firstToken() );
        }
    }

    Expr::Expr(Parser::BinaryExpression *expr)
    {
        std::cout << "Expr: Generating abstract binary expr\n";
        op = expr->op;
        
        ParseTreeConverter visitor = ParseTreeConverter();
        if (expr->expr != nullptr )
        {

            std::cout << "Expr: Visiting LHS expr\n";
            expr->expr->accept(&visitor);
            left = visitor.pNode;

            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->expr->firstToken() );
            visitor.pNode = nullptr;
        }

        if ( expr->right != nullptr )
        {
            std::cout << "Expr: Visiting RHS expr\n";
            expr->right->accept(&visitor);
            right = visitor.pNode;
            if (visitor.pNode == nullptr)
                throw Parser::ParseException( expr->right->firstToken() );
            visitor.pNode = nullptr;
        }
    }


    Assign::Assign(Parser::AssignExpression *expr)
        : Expr(expr)
    {
        std::cout << "Assign: Generating expr\n";
    }

    Add::Add(Parser::ArithmeticExpression *expr)
        : Expr(expr)
    {
        std::cout << "Add: Generating expr\n";
    }


    Return::Return(Parser::ReturnStmt *stmt)
    {
        std::cout << "Generating return statement node\n";
        if ( stmt->expr != nullptr )
        {
            // expr = new Expr
            ParseTreeConverter visitor = ParseTreeConverter();

            stmt->expr->accept(&visitor);
            Node *pNode(visitor.pNode);

            if ( pNode == nullptr )
                std::cout << "Skipping expr: " << stmt->expr->firstToken();
        }
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
        std::cout << "StmtBlock: Found " << vars << " vars in block" << std::endl;

        for( auto &stmt: block->stmts )
        {
            ParseTreeConverter visitor = ParseTreeConverter();
            // visit node and covnert

            std::cout << "Asking node to accept visitor\n";
            stmt->accept(&visitor);
            std::cout << "Checking if visitor was able to convert to AST node\n";
            Node *pNode( visitor.pNode );
            // make decisions about what type of stmt
            // keywords first
        

            if (pNode != nullptr)
            {
                stmts.push_back(pNode);
            }
            else
                throw Parser::ParseException( stmt->firstToken() );
            // std::cout << "Skipping stmt: " << stmt->firstToken();
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
        std::cout << ident.getValue<std::string>() << ": Found " << formals.size() << " formals" << std::endl;

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

        std::cout << "Program: Found " << decls.size() << " declarations" << std::endl;


        return;
    }
} // namespace AST
