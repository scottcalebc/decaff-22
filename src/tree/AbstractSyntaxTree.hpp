#pragma once

#include "token/token.hpp"

namespace AST {

    class Node {
        protected:            
            Node() {};
            
        public:
            // future meta information for symbol table, return type (function/expr), etc.
            
            // visitor acceptor method for semantic, type, and code gen
            template< typename Visitor>
            void accept(Visitor visitor) { visitor.visit(this); };
    }

    // represents either identifier or constant
    class Value: public Node
    {
        public:
            Scanner::Token              value;
    }

    class StatementBlock: public Node
    {
        public:
            std::vector<Declaration*>   decls;
            std::vector<Node*>          stmts;
    }


    class Declaration: public Node
    {
        public:
            Scanner::Token::Type        type;
            Scanner::Token              ident;
    }

    class FunctionDeclaration: public Node
    {
        public:
            std::vector<Declaration*>   formals;
    }

    class Expr: public Node
    {
        public:
            // Binary expressions will have both left and right as non null
            // Unary expressions will have right as non null
            // Values will have no op (i.e. empty)
            Node*                       left;
            Scanner::Token              op;
            Node*                       right;
    }




}