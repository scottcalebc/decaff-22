#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>


namespace SemanticAnalyzer {

    class STTypeVisitor: public Visitor {

        public:
            STTypeVisitor(): inLoop(false), err(false) {};

            bool inLoop;
            bool err;       // Set if error occurs, this will prevent code gen

            void printTypeError(Scanner::Token token, std::string errStr);

            bool arithmeticCheck(Scanner::Token::Type type);

            bool binaryTypeCheck(AST::Node* left, AST::Node*right, bool unary=false);
            // bool unaryTypeCheck(AST::Node* left);

            // default acceptor to show missing virtual functions
            void visit(Acceptor *a) { std::cout << "STTypeVisitor: Got acceptor" << std::endl; };

            // Ident will lookup in ST and return type
            void visit(AST::Ident *p);
            // Constant will simply set their type
            void visit(AST::Constant *p);

            // Expressions

            // arithmetic expressions
            void visit(AST::Add *p);
            void visit(AST::Assign *p);
            void visit(AST::Divide *p);
            void visit(AST::Modulus *p);
            void visit(AST::Subtract *p);
            void visit(AST::Multiply *p);

            // logical  epxressions
            void visit(AST::Or *p);
            void visit(AST::And *p);
            void visit(AST::Not *p);
            void visit(AST::GTE *p);
            void visit(AST::LTE *p);
            void visit(AST::Equal *p);
            void visit(AST::NotEqual *p);
            void visit(AST::LessThan *p);
            void visit(AST::GreaterThan *p);

            // function calls
            void visit(AST::Call *p);
            void visit(AST::Print *p);
            void visit(AST::ReadLine *p);
            void visit(AST::ReadInteger *p);
            
            // keyword statments, break has no outtype
            void visit(AST::If *p);
            void visit(AST::For *p);
            void visit(AST::Break *p);
            void visit(AST::While *p);
            void visit(AST::Return *p);
            void visit(AST::KeywordStmt *p) {};


            void visit(AST::StatementBlock *p);
            void visit(AST::FunctionDeclaration *p);
            void visit(AST::Program *p);
    };


    bool typeCheck(AST::Program *p);

};