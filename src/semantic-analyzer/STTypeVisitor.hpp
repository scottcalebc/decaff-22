#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>


namespace SemanticAnalyzer {

    class STTypeVisitor: public Visitor {

        public:
            STTypeVisitor() {};

            void visit(Acceptor *a) { std::cout << "STTypeVisitor: Got acceptor" << std::endl; };

            
            void visit(AST::FunctionDeclaration *p) {};
            void visit(AST::Program *p) {};
            void visit(AST::StatementBlock *p) {};
            void visit(AST::KeywordStmt *p) {};
            void visit(AST::While *p) {};
            void visit(AST::For *p) {};
            void visit(AST::If *p) {};
    };

};