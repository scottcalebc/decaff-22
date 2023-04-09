#include <iostream>

#include <parser/exceptions.hpp>

#include "AbstractSyntaxTree.hpp"
#include "ParseTreeVisitor.hpp"



namespace AST
{
    // Node * ExprParseNode(Parser::Expression *parseNode)
    // {

    //     if ( dynamic_cast<Parser::LValue*>(parseNode) != nullptr)
    //     {
    //         return new Ident(dynamic_cast<Parser::LValue*>(parseNode)->ident->ident);
    //     }
    //     if ( dynamic_cast<Parser::Constant*>(parseNode) != nullptr )
    //     {
    //         return new Constant(dynamic_cast<Parser::Constant*>(parseNode)->constant);
    //     }
    //     if ( dynamic_cast<Parser::CallExpression*>(parseNode) != nullptr )
    //     {

    //     }

    // }

    // void ParseTreeVisitor::visit(Parser::ParseNode *parseNode)
    // {
    //     std::cout << "Visit called by abstract parse node\n";
    // }

    // void ParseTreeVisitor::visit(Parser::BreakStmt *parseNode)
    // {
    //     std::cout << "Visit called by break statement\n";
    //     pNode = new Break();
    // }

    // void ParseTreeVisitor::visit(Parser::ReturnStmt *parseNode)
    // {
    //     std::cout << "Visit called by Return statement\n";
    //     pNode = new Return(parseNode);
    // }
    Return::Return(Parser::ReturnStmt *stmt)
    {
        if ( stmt->expr != nullptr )
        {
            // expr = new Expr
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
            // stmt->accept(visitor);
            std::cout << "Checking if visitor was able to convert to AST node\n";
            Node *pNode( visitor.pNode );
            // make decisions about what type of stmt
            // keywords first
        

            if (pNode != nullptr)
                stmts.push_back(pNode);
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

        }

        std::cout << "Program: Found " << decls.size() << " declarations" << std::endl;


        return;
    }
} // namespace AST
