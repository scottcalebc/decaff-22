#pragma once

#include <visitor/visitor.hpp>

#include <parser/ParseTree.hpp>
#include <token/token.hpp>

namespace AST {

    /**
     *      Generic Syntax Tree Objects
     */
    class Node : public Acceptor {
        protected:            
            Node() {};
            
        public:
            // future meta information for symbol table, return type (function/expr), etc.
            
            // // visitor acceptor method for semantic, type, and code gen
            // template< typename Visitor>
            // void accept(Visitor visitor) { visitor.visit(this); };
            // virtual void accept(Visitor *visitor) { visitor->visit(this); };
            virtual void accept() { };
    };

    // represents either identifier or constant
    class Value: public Node
    {
        public:
            Value()
                : Node()
                , value()
                {};

            Value(Scanner::Token token)
                : Node()
                , value(token)
            {};
            
            Scanner::Token              value;
    };

    class Declaration: public Node
    {
        public:
            Declaration()
                : Node()
                , type()
                , ident()
                {};

            Declaration(Parser::Declarations *decl)
                : Node()
                , type(decl->type->type.type)
                , ident(decl->ident->ident)
            {};
            
            Scanner::Token::Type        type;
            Scanner::Token              ident;
    };

    /**
     * @brief Collect variable decls and statements in one object
     * 
     * Cannot include this under a single funciton declaration object
     *  due to the fact that blocks have their own scope, by having this
     *  object we are able to provide it's own symbol table scope
     * 
     */
    class StatementBlock: public Node
    {
        public:
            StatementBlock()
                : Node()
                , decls()
                , stmts()
                {};

            StatementBlock(Parser::StatementBlock *block);
            
            // is this even needed? parsing already ensured that decls are at the front
            std::vector<Declaration*>   decls;  
            std::vector<Node*>          stmts;
    };
    class FunctionDeclaration: public Declaration
    {
        public:
            FunctionDeclaration()
                : Declaration()
                , formals()
                , stmts(nullptr)
                {};

            FunctionDeclaration(Parser::FunctionDeclaration *func);
            
            std::vector<Declaration*>   formals;
            StatementBlock*             stmts;
    };

    class Ident: public Value
    {
        public:
            Ident(Scanner::Token token)
                : Value(token)
                {};
    };

    class Constant: public Value
    {
        public:
            Constant(Scanner::Token token)
                : Value(token)
                {};
    };

    class Call: public Value
    {
        public:
            Call()
                : Value()
                , actuals()
                {};
            
            std::deque<Node> actuals;
    };

    class Expr: public Node
    {
        public:
            Expr()
                : Node()
                , left(nullptr)
                , op(Scanner::Token::Type::EMPTY)
                , right(nullptr)
                {};

            // Binary expressions will have both left and right as non null
            // Unary expressions will have right as non null
            // Values/calls will have no op (i.e. empty)
            Node*                       left;
            Scanner::Token              op; // may not need this
            Node*                       right;
    };

    class KeywordStmt: public Value
    {
        public:
            KeywordStmt()
                : Value()
                , expr(nullptr)
                {};

            // for most keywords this will be conditional expression
            // for return this will be return value or null/void
            // for break this should be null
            Node* expr;
    };


    /**
     *  Specific Syntax nodes (i.e. Add, Subtract, etc.)
     *  this should help when performing semantic/code gen
     * 
     */


    // Arithmetic Objects
    class Add: public Expr
    {
        public:
            Add()
                : Expr()
            {};

            Add(Parser::ArithmeticExpression *expr);
    };

    // If right is null this will be unary minus
    class Subtract: public Expr
    {};

    class Multiply: public Expr
    {};

    class Divide: public Expr
    {};

    // Boolean or conditional objects
    class LessThan: public Expr
    {};

    class LTE: public LessThan
    {};

    class GreaterThan: public Expr
    {};

    class GTE: public Expr
    {};

    class And: public Expr
    {};

    class Or: public Expr
    {};

    class Not: public Expr
    {};

    class Assign: public Expr
    {};

    // Keyword statements
    class Break: public KeywordStmt
    {};

    class Return: public KeywordStmt
    {
        public:
            Return()
                : KeywordStmt()
                {};

            Return(Parser::ReturnStmt *smt);
    };

    class While: public KeywordStmt
    {
        public:
            While()
                : KeywordStmt()
                , stmt(nullptr)
                {};
            
            Node* stmt; // this will be singluar statment or statement block
    };

    class If: public While
    {
        public:
            If()
                : While()
                , elseStmt(nullptr)
                {};
            Node* elseStmt; // this will hold else statement/block
    };

    class For: public While
    {
        public:
            For()
                : While()
                , startExpr(nullptr)
                , loopExpr(nullptr)
                {};
            
            Expr* startExpr;
            Expr* loopExpr;
    };

    // Builtin Function objects
    class Print: public Call
    {};

    class ReadInteger: public Call
    {};

    class ReadLine: public Call
    {};

    class Program: public Node
    {
        public:
            Program()
                : Node()
                , decls()
                {};

            Program(Parser::Program *p);
            
            // std::vector<Declaration*> vars;
            // std::vector<FunctionDeclaration*> func;

            std::vector<Node*> decls;
    };

    // Node * ExprParseNode(Parser::Expression *parseNode);


}