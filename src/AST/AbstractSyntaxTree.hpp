#pragma once

#include <visitor/parseVisitor.hpp>
#include <visitor/astVisitor.hpp>

#include <parser/ParseTree.hpp>
#include <token/token.hpp>

#include <SymbolTable/Entities.hpp>

#include <code-gen/Entities.hpp>

namespace AST {

    /**
     *      Generic Syntax Tree Objects
     */
    class Node : public Acceptor {
        protected:            
            Node() 
                : pScope(nullptr)
                , reg(nullptr)
            {};
            
        public:
            // future meta information for symbol table, return type (function/expr), etc.

            // Each node holds a ref to their closest scope, each scope holds a ref to their
            // parent thus preserving static scoping rules
            SymbolTable::Scope *pScope;

            // Out register holding value of sub expression
            CodeGen::Register   *reg;
            CodeGen::Memory     *mem; // holds memory location of subexpression

            std::string memName;

            // Out type will be used by expressions to verify type of operation
            // Will throw error if type mismatch
            Scanner::Token::Type outType;

            // // visitor acceptor method for semantic, type, and code gen
            virtual void accept(Visitor* v) = 0;

            // virtual method to distribute scopes to children nodes
            // this is to ensure we don't have to hit every object with visitor
            // to set its local scope
            virtual void setScope(SymbolTable::Scope *pScope) = 0;

            /**
             * @brief Get the min col start for node and it's children
             * 
             * @return int 
             */
            virtual int minCol() = 0;
            /**
             * @brief Get the max col start for node and it's children
             * 
             * @return int 
             */
            virtual int maxCol() = 0;
            
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

            virtual void setScope(SymbolTable::Scope *p) { pScope = p; };

            // helper functions for min max columen info
            virtual int minCol() { return value.colStart; };
            virtual int maxCol() { return value.colStart + value.getValue<std::string>().length(); };
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

            virtual void accept(Visitor *v) { v->visit(this); };
            
            Scanner::Token::Type        type;
            Scanner::Token              ident;

            void setScope(SymbolTable::Scope *p) { pScope = p; };

            virtual int minCol() { return ident.colStart; };
            virtual int maxCol() { return ident.colStart + ident.getValue<std::string>().length(); };
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
            void accept(Visitor *v) { v->visit(this); };
            
            // is this even needed? parsing already ensured that decls are at the front
            std::vector<Declaration*>   decls;  
            std::vector<Node*>          stmts;

            void setScope(SymbolTable::Scope *p);

            int minCol() { return decls.front()->minCol(); };
            int maxCol() { return stmts.back()->maxCol();  };
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
            void accept(Visitor *v) 
            { 
                // std::cout << "Function accepting visitor: "; 
                v->visit(this); 
            };
            
            std::vector<Declaration*>   formals;
            StatementBlock*             stmts;

            void setScope(SymbolTable::Scope *p);
    };

    class Ident: public Value
    {
        public:
            Ident(Scanner::Token token)
                : Value(token)
                {
                    // std::cout << "Identifier: " << token.getValue<std::string>() << std::endl;
                };

            void accept(Visitor *v) { v->visit(this); };
    };

    class Constant: public Value
    {
        public:
            Constant(Scanner::Token token)
                : Value(token)
                {
                    // std::cout << "Constant: " << token.getValue<std::string>() << std::endl;
                };

            void accept(Visitor *v) { v->visit(this); };
    };

    class Call: public Value
    {
        public:
            Call()
                : Value()
                , actuals()
                {};

            Call(Parser::CallExpression *c);
            void accept(Visitor *v) { v->visit(this); };
            
            std::deque<Node*> actuals;
            void setScope(SymbolTable::Scope *p)
            {
                pScope = p;
                for (auto node: actuals)
                {
                    node->setScope(p);
                }
            };

            int maxCol() { return actuals.back()->maxCol() + 1; };
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
            Expr(Parser::UnaryExpression *expr);
            Expr(Parser::BinaryExpression *exp);

            // Binary expressions will have both left and right as non null
            // Unary expressions will have right as non null
            // Values/calls will have no op (i.e. empty)
            Node*                       left;
            Scanner::Token              op; // may not need this
            Node*                       right;

            virtual int minCol()
            {
                int lmin( -1 );
                
                if (left != nullptr)
                    lmin = left->minCol();

                int rmin( lmin + 1 );

                if (right != nullptr)
                    rmin = right->minCol();

                return std::min(lmin, rmin);
            }

            virtual int maxCol()
            {
                int lmax( -1 );

                if (left != nullptr)
                    lmax = left->maxCol();

                int rmax( lmax - 1 );

                if (right != nullptr)
                    rmax = right->maxCol();

                return std::max(lmax, rmax);
            }

            void setScope(SymbolTable::Scope *p)
            {
                pScope = p;
                if (left != nullptr)
                    left->setScope(p);
                if (right != nullptr)
                    right->setScope(p);
            };
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
            void setScope(SymbolTable::Scope *p);
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
                // std::cout << "Add: Generating expr\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Subtract: Generating expr\n";
            };

            Subtract(Parser::UnaryExpression *expr)
                : Expr(expr)
            {
                // std::cout << "Subtract: Generating unary minus\n";
                // std::cout << "Left should be non null: " << &left << std::endl;
                // std::cout << "Right should be null   : " << &right << std::endl;
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Multiply: Generating expr\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Divide: Generating expr\n";
            };

            void accept(Visitor *v) { v->visit(this); };
    };

    class Modulus: public Expr
    {
        public:
            Modulus();
            
            Modulus(Parser::ArithmeticExpression *expr);

            void accept(Visitor *v);
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
                // std::cout << "LessThan: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "LessThanEqual: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "GreaterThan: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "GreaterThanEqual: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Equal: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "NotEqual: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "And: generating\n";
            };
            
            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Or: generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Not: Generating expr\n";
            };

            void accept(Visitor *v) { v->visit(this); };
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
                // std::cout << "Assign: Generating expr\n";
            };

            void accept(Visitor *v) { v->visit(this); };
    };

    // Keyword statements
    class Break: public KeywordStmt
    {
        public:
            Break()
                : KeywordStmt()
                {};

            Break(Parser::BreakStmt *stmt) : KeywordStmt(stmt) {};
            void accept(Visitor *v) { v->visit(this); };
    };

    class Return: public KeywordStmt
    {
        public:
            Return()
                : KeywordStmt()
                {};

            Return(Parser::ReturnStmt *stmt);

            void accept(Visitor *v) { v->visit(this); };
    };

    class While: public KeywordStmt
    {
        public:
            While()
                : KeywordStmt()
                , stmt(nullptr)
                {};

            While(Parser::WhileStmt *stmt);
            void accept(Visitor *v) { v->visit(this); };
            void setScope(SymbolTable::Scope *p);
            
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
            void accept(Visitor *v) { v->visit(this); };
            void setScope(SymbolTable::Scope *p);

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
            void accept(Visitor *v) { v->visit(this); };
            void setScope(SymbolTable::Scope *p);
            
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
                // std::cout << "Print: Generating\n";
            };

            void accept(Visitor *v) { v->visit(this); };
    };

    class ReadInteger: public Call
    {
        public:
            ReadInteger() : Call(){};
            ReadInteger(Parser::ReadIntExpr *p) : Call(p) 
            {
                // std::cout << "ReadInteger: Generating\n"; 
            };
            void accept(Visitor *v) { v->visit(this); };
            int minCol() { return value.colStart; };
            int maxCol() { return value.colStart + value.getValue<std::string>().length() + 2; };
    };

    class ReadLine: public Call
    {
        public:
            ReadLine() : Call(){};
            ReadLine(Parser::ReadLineExpr *p) : Call(p) 
            {
                // std::cout << "ReadInteger: Generating\n"; 
            };
            void accept(Visitor *v) { v->visit(this); };
            int minCol() { return value.colStart; };
            int maxCol() { return value.colStart + value.getValue<std::string>().length() + 2; };
    };

    class Program: public Node
    {
        public:
            Program()
                : Node()
                , vars()
                , func()
                {};

            Program(Parser::Program *p);

            void accept(Visitor *v) { v->visit(this); };
            void setScope(SymbolTable::Scope *p);
            
            std::vector<Declaration*> vars;
            std::vector<Node*> func;

            // std::vector<Node*> decls;
            int minCol() { return 0; };
            int maxCol() { return func.back()->maxCol(); };
    };

}