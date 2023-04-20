#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>


namespace SemanticAnalyzer {

    class STTypeVisitor: public Visitor {

        public:
            STTypeVisitor() {};

            bool binaryTypeCheck(AST::Node* left, AST::Node*right, bool unary=false);
            // bool unaryTypeCheck(AST::Node* left);

            // default acceptor to show missing virtual functions
            void visit(Acceptor *a) { std::cout << "STTypeVisitor: Got acceptor" << std::endl; };

            // Ident will lookup in ST and return type
            void visit(AST::Ident *p);
            // Constant will simply set their type
            void visit(AST::Constant *p);

            // Expressions
            void visit(AST::Add *p);
            void visit(AST::Assign *p);
            void visit(AST::Divide *p);
            void visit(AST::Subtract *p);
            void visit(AST::Multiply *p);
            
            void visit(AST::KeywordStmt *p) {};
            void visit(AST::While *p) {};
            void visit(AST::For *p) {};
            void visit(AST::If *p) {};


            void visit(AST::StatementBlock *p);
            void visit(AST::FunctionDeclaration *p);
            void visit(AST::Program *p);
    };


    void typeCheck(AST::Program *p);

};