#pragma once

#include <visitor/parseVisitor.hpp>
#include <visitor/astVisitor.hpp>

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
            virtual void accept(Visitor* v) { v->visit(this); };
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
                {
                    std::cout << "Identifier: " << token.getValue<std::string>() << std::endl;
                };
    };

    class Constant: public Value
    {
        public:
            Constant(Scanner::Token token)
                : Value(token)
                {
                    std::cout << "Constant: " << token.getValue<std::string>() << std::endl;
                };
    };

    class Call: public Value
    {
        public:
            Call()
                : Value()
                , actuals()
                {};

            Call(Parser::CallExpression *c);
            
            std::deque<Node*> actuals;
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

            Expr(Parser::Expression *expr);
            Expr(Parser::BinaryExpression *exp);

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

            KeywordStmt(Parser::KeywordStmt *stmt);

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

            Add(Parser::ArithmeticExpression *expr)
            : Expr(expr)
            {
                std::cout << "Add: Generating expr\n";
            };
    };

    // If right is null this will be unary minus
    class Subtract: public Expr
    {
        public:
            Subtract()
                : Expr()
            {};
            
            Subtract(Parser::ArithmeticExpression *expr)
                : Expr(expr)
            {
                std::cout << "Subtract: Generating expr\n";
            };

            Subtract(Parser::UnaryExpression *expr)
                : Expr(expr)
            {
                std::cout << "Subtract: Generating unary minus\n";
            };
    };

    class Multiply: public Expr
    {
        public:
            Multiply()
                : Expr()
            {};
            
            Multiply(Parser::ArithmeticExpression *expr)
                : Expr(expr)
            {
                std::cout << "Multiply: Generating expr\n";
            };
    };

    class Divide: public Expr
    {
        public:
            Divide()
                : Expr()
            {};
            
            Divide(Parser::ArithmeticExpression *expr)
                : Expr(expr)
            {
                std::cout << "Divide: Generating expr\n";
            };
    };

    class Modulus: public Expr
    {
        public:
            Modulus()
                : Expr()
            {};
            
            Modulus(Parser::ArithmeticExpression *expr)
                : Expr(expr)
            {
                std::cout << "Modulus: Generating expr\n";
            };
    };

    // Boolean or conditional objects
    class LessThan: public Expr
    {
        public:
            LessThan()
                : Expr()
                {};
            
            LessThan(Parser::RelationalExpression *expr)
                : Expr(expr)
            {
                std::cout << "LessThan: generating\n";
            }
    };

    class LTE: public LessThan
    {
        public:
            LTE()
                : LessThan()
                {};
            
            LTE(Parser::RelationalExpression *expr)
                : LessThan(expr)
            {
                std::cout << "LessThanEqual: generating\n";
            }
    };

    class GreaterThan: public Expr
    {
        public:
            GreaterThan()
                : Expr()
                {};
            
            GreaterThan(Parser::RelationalExpression *expr)
                : Expr(expr)
            {
                std::cout << "GreaterThan: generating\n";
            }
    };

    class GTE: public GreaterThan
    {
        public:
            GTE()
                : GreaterThan()
                {};
            
            GTE(Parser::RelationalExpression *expr)
                : GreaterThan(expr)
            {
                std::cout << "GreaterThanEqual: generating\n";
            }
    };

    class Equal: public Expr
    {
        public:
            Equal()
                : Expr()
                {};
            
            Equal(Parser::EqualityExpression *expr)
                : Expr(expr)
            {
                std::cout << "Equal: generating\n";
            }
    };

    class NotEqual: public Expr
    {
        public:
            NotEqual()
                : Expr()
                {};
            
            NotEqual(Parser::EqualityExpression *expr)
                : Expr(expr)
            {
                std::cout << "NotEqual: generating\n";
            }
    };

    class And: public Expr
    {
        public:
            And()
                : Expr()
                {};
            
            And(Parser::LogicalExpression *expr)
                : Expr(expr)
            {
                std::cout << "And: generating\n";
            };
    };

    class Or: public Expr
    {
        public:
            Or()
                : Expr()
                {};
            
            Or(Parser::LogicalExpression *expr)
                : Expr(expr)
            {
                std::cout << "Or: generating\n";
            };
    };

    class Not: public Expr
    {
        public:
            Not()
                : Expr()
                {};
            
            Not(Parser::UnaryExpression *expr)
                : Expr(expr)
            {
                std::cout << "Not: Generating expr\n";
            };
    };

    class Assign: public Expr
    {
        public:
            Assign()
                : Expr()
            {};

            Assign(Parser::AssignExpression *expr)
                : Expr(expr)
            {
                std::cout << "Assign: Generating expr\n";
            };
    };

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

            While(Parser::WhileStmt *stmt);
            
            Node* stmt; // this will be singluar statment or statement block
    };

    class If: public While
    {
        public:
            If()
                : While()
                , elseStmt(nullptr)
                {};

            If(Parser::IfStmt *stmt);
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

            For(Parser::ForStmt *stmt);
            
            Node* startExpr;
            Node* loopExpr;
    };

    // Builtin Function objects
    class Print: public Call
    {
        public:
            Print()
                :Call()
                {};
            
            Print(Parser::PrintStmt *p)
                : Call(p)
            {
                std::cout << "Print: Generating\n";
            };
    };

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