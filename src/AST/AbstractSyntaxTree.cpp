#include <iostream>

#include "AbstractSyntaxTree.hpp"



namespace AST
{

    StatementBlock::StatementBlock(Parser::StatementBlock *block)
        : Node()
        , stmts()
    {
        for (auto &decl : block->vars)
        {
            stmts.push_back(new Declaration(decl));
        }

        int vars( stmts.size() );
        std::cout << "StmtBlock: Found " << vars << " vars in block" << std::endl;
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
        , vars()
        , func()
    {
        for(auto &decl : p->decls)
        {
            if (dynamic_cast<Parser::FunctionDeclaration*>(decl) != nullptr) 
                func.push_back(new FunctionDeclaration(dynamic_cast<Parser::FunctionDeclaration*>(decl)) );
            else
                vars.push_back( new Declaration(decl) );

        }

        std::cout << "Program: Found " << vars.size() << " variable declarations" << std::endl;
        std::cout << "Program: Found " << func.size() << " function declarations" << std::endl;


        return;
    }
} // namespace AST
