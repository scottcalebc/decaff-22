#pragma once

#include <common/ASTForward.hpp>


class Acceptor;

/**
 * @brief Visitor interface that will visit nodes that have an acceptor function
 * 
 */
class Visitor {

    protected:
        Visitor() {};

    public:
        virtual void visit(Acceptor *a) = 0;
        virtual void visit(AST::If *p) = 0;
        virtual void visit(AST::Add *p) = 0;
        virtual void visit(AST::For *p) = 0;
        virtual void visit(AST::While *p) = 0;
        virtual void visit(AST::Ident *p) = 0;
        virtual void visit(AST::Divide *p) = 0;
        virtual void visit(AST::Assign *p) = 0;
        virtual void visit(AST::Program *p) = 0;
        virtual void visit(AST::Multiply *p) = 0;
        virtual void visit(AST::Subtract *p) = 0;
        virtual void visit(AST::Constant *p) = 0;
        virtual void visit(AST::KeywordStmt *p) = 0;
        virtual void visit(AST::StatementBlock *p) = 0;
        virtual void visit(AST::FunctionDeclaration *p) = 0;
};



/**
 * @brief Acceptor interface that will accept someone from the Visitor Interface
 * 
 */
class Acceptor {
    protected:
        Acceptor() {};

    public:
        virtual void accept(Visitor *v) = 0;
};
