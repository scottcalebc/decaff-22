

#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>


#include "Entities.hpp"

namespace SymbolTable
{
    class STVisitor: public Visitor {
            public:
                STVisitor() 
                        : currScope(nullptr)
                {};

                Scope* currScope;

                // default acceptor may remove
                void visit(Acceptor *a) 
                { 
                        std::cout << "Got acceptor"; 
                        if (dynamic_cast<AST::FunctionDeclaration *>(a) != nullptr)
                                std::cout << " w/ function decl";

                        std::cout << std::endl;
                };

                // non-used virtual overrides
                void visit(AST::Add *p) {};
                void visit(AST::Ident *p) {};
                void visit(AST::Assign *p) {};
                void visit(AST::Subtract *p) {};
                void visit(AST::Multiply *p) {};
                void visit(AST::Divide *p) {};
                void visit(AST::Constant *p) {};


                // actual implemented overrides
                void visit(AST::FunctionDeclaration *p);
                void visit(AST::Program *p);
                void visit(AST::StatementBlock *p);
                void visit(AST::KeywordStmt *p);
                void visit(AST::While *p);
                void visit(AST::For *p);
                void visit(AST::If *p);
        };
}