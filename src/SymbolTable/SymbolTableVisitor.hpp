

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

                void visit(Acceptor *a) 
                { 
                        std::cout << "Got acceptor"; 
                        if (dynamic_cast<AST::FunctionDeclaration *>(a) != nullptr)
                                std::cout << " w/ function decl";

                        std::cout << std::endl;
                };


                void visit(AST::FunctionDeclaration *p);
                void visit(AST::Program *p);
                void visit(AST::StatementBlock *p);
        };
}