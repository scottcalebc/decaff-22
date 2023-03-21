#pragma once

#include <vector>
#include <deque>
#include "token.hpp"

/** Non-Abstract / Derived Classes */
/**
 * Pure Abstract defined classes
 */
class ParseNode
{
    protected:
        ParseNode()
        {};
        virtual std::string nodeName() = 0;

    public:
        virtual int line() = 0;
        virtual std::string toString(int numSpaces) = 0;
        virtual Scanner::Token firstToken() = 0;
};
class Identifier : public ParseNode
{
    public:
        Scanner::Token ident;

        Identifier()
            : ParseNode()
            , ident()
        {

        };

        Identifier(const Identifier &i)
        {
            ident = i.ident;
        }

        int line()
        {
            return ident.lineNumber;
        }
        std::string nodeName() { return "Identifier: "; };
        std::string toString(int numSpaces);

        Scanner::Token firstToken() { return ident; };
};

class DeclarationType : public ParseNode
{
    public:
        DeclarationType()
            : ParseNode()
            , type()
        {

        };

        DeclarationType(const DeclarationType & dt)
        {
            type = dt.type;
        }

        Scanner::Token type;

        int line()
        {
            return type.lineNumber;
        }
        std::string nodeName() { return "Type: "; };
        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return type; };
};

class ReturnType: public DeclarationType
{
    public:
        ReturnType()
            : DeclarationType()
        {
            
        };
        std::string nodeName() { return "(return type) " + DeclarationType::nodeName(); };
};

class Statement : public ParseNode
{
    protected:
        Statement()
        {

        };

    public:
        virtual ~Statement()
        {

        };
        virtual int line() { return 0; };
        virtual std::string nodeName() { return "Stmt: ";};
        virtual std::string toString(int numSpaces);
};



class Declarations : public ParseNode
{
    protected:
        Declarations()
            : type(nullptr)
            , ident()
        {

        };

    public:
        DeclarationType *type;
        Identifier      *ident;
        virtual ~Declarations() {
            delete type;
            delete ident;
        };

        int line()
        {
            return ident->line();
        };

        virtual std::string toString(int numSpaces);
        virtual Scanner::Token firstToken() { return type->firstToken(); };
};

class Expression : public Statement
{
    public:
        Expression()
            : expr(nullptr)
        {

        };
        virtual ~Expression()
        {
            delete expr;
        };
        Expression *expr;
        Scanner::Token semiColon;

        virtual int line() {return semiColon.lineNumber; };
        virtual std::string toString(int numSpaces);
        virtual Scanner::Token firstToken() { return (expr != nullptr) ? expr->firstToken() : semiColon; };
};

class BinaryExpression : public Expression
{
    public:
        Scanner::Token op;
        Expression *right;

        BinaryExpression()
            : Expression()
            , op()
            , right(nullptr)
        {

        };

        ~BinaryExpression()
        {
            delete right;
        };

        template<typename T>
        bool followExpr(T* follow) { return true; };

        virtual int line() { return op.lineNumber; };
        std::string nodeName() { return "BinExpr: "; };
        virtual std::string toString(int numSpaces);
        Scanner::Token firstToken() { return expr->firstToken(); };
};

class UnaryExpression : public BinaryExpression
{
    public:
        UnaryExpression()
            : BinaryExpression()
        {

        };

        std::string nodeName() { return "UnaryExpr:"; };
};



/**
 * Derived Classes for Actual Implementation / Derivation of Parse Tree
 */

class AssignExpression : public BinaryExpression
{
    public:
        std::string nodeName() { return "AssignExpr: "; };
};

class ArithmeticExpression : public BinaryExpression
{
    public:
        std::string nodeName() { return "ArithmeticExpr:"; };
};


class LogicalExpression : public BinaryExpression
{
    public:
        std::string nodeName() { return "LogicalExpr:"; };

        bool followExpr(LogicalExpression* follow) { 
            if (follow != nullptr)
                return false; 
            else
                return true;
                };
};




class ParenExpr : public Expression
{
    public:
        Scanner::Token lparen;
        Scanner::Token rparen;

        ParenExpr()
            : Expression()
            , lparen()
            , rparen()
        {

        };

        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return lparen; };
};



class LValue : public Expression
{

    public:
        Identifier *ident;

        LValue()
            : Expression()
            , ident(nullptr)
        {

        };

        ~LValue()
        {
            delete ident;
        }

        int line() { return ident->line(); };
        std::string nodeName() { return "FieldAccess: "; };
        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return ident->firstToken(); };
};

class Constant : public Expression
{
    public:
        Scanner::Token constant;

        Constant()
            : Expression()
            , constant()
        {

        };

        int line() { return constant.lineNumber; };
        std::string nodeName() { return Scanner::Token::getTypeName(constant.type) + ":"; }; 
        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return constant; };
};

/**
 * @brief Variable declaration separate from Function declaration
 *      Contains the following additional items
 *           ';' After previously listed tokens/Tree nodes
 * 
 */
class VariableDeclaration : public Declarations
{
    public:
        Scanner::Token semiColon;

        VariableDeclaration() 
            : Declarations()
            , semiColon()
        {

        };

        std::string nodeName() { return "VarDecl:"; };
};


class StatementBlock : public Statement
{
    public:
        Scanner::Token lbrace;
        std::vector<VariableDeclaration*>   vars;
        std::vector<Statement*>            stmts;
        Scanner::Token rbrace;
        
        StatementBlock()
            : Statement()
            , lbrace()
            , vars()
            , stmts()
        {

        };

        ~StatementBlock()
        {
            for (auto var : vars)
            {
                delete var;
            }

            for (auto stmt : stmts)
            {
                delete stmt;
            }
        }

        std::string nodeName() { return "StmtBlock: "; };
        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return lbrace; };
};


class FormalVariableDeclaration : public VariableDeclaration
{
    public:
        FormalVariableDeclaration() 
            : VariableDeclaration()
        {

        };

        FormalVariableDeclaration(VariableDeclaration*var);
        std::string nodeName() { return "(formals) " + VariableDeclaration::nodeName(); };
};

class FunctionDeclaration : public Declarations
{
    public:
        Scanner::Token                              lparen;
        /*vector of formals since can be 0-inf (in theory) */
        std::vector<FormalVariableDeclaration*>     formals;
        Scanner::Token                              rparen;
        StatementBlock                              *block;

        FunctionDeclaration() 
            : Declarations()
            , lparen()
            , formals()
            , rparen()
        {

        };

        ~FunctionDeclaration()
        {
            for( auto formal : formals)
            {
                delete formal;
            }
        };


        std::string nodeName() { return "FnDecl:"; };
        std::string toString(int numSpaces);
        
};

class Actual: public Expression
{
    public:

        std::string nodeName() { return "(actual) " + expr->nodeName(); };
};

class CallExpression : public Expression
{
    public:
        Identifier* ident;
        std::deque<Expression*> actuals;
        Scanner::Token lparen;
        Scanner::Token rparen;

        CallExpression() 
            : Expression()
            , ident(nullptr)
            , actuals()
            , lparen()
            , rparen()
        {

        };

        int line() { return lparen.lineNumber; };
        std::string nodeName() { return "Call: "; };
        std::string toString(int numSpaces);
        Scanner::Token firstToken() { return ident->firstToken(); };
};

class PrintStmt : public CallExpression
{
    public:

        PrintStmt()
            : CallExpression()
        {
            
        };
        PrintStmt(CallExpression *expr);

        std::string nodeName() { return "PrintStmt: "; };
        std::string toString(int numSpaces);
};

class ReadIntExpr : public CallExpression
{
    public:
    ReadIntExpr()
            : CallExpression()
        {

        };

    std::string toString(int numSpaces);
    std::string nodeName() { return "ReadIntegerExpr: "; };
};

class ReadLineExpr : public CallExpression
{
    public:
        ReadLineExpr()
            : CallExpression()
        {

        };
        
    std::string toString(int numSpaces);
    std::string nodeName() { return "ReadLineExpr: "; };
};


class KeywordStmt: public Statement
{
    public:
        Scanner::Token keyword;
        Expression* expr;
        KeywordStmt()
            : Statement()
            , keyword()
            , expr(nullptr)
        {

        };

        virtual int line() { return keyword.lineNumber; };
        virtual std::string nodeName() { return "KeywordStmt: "; };
        virtual std::string toString(int numSpaces);
        Scanner::Token firstToken() { return keyword; };

};


class ReturnStmt : public KeywordStmt
{
    public:
        ReturnStmt()
            : KeywordStmt()
            , semiColon()
            {

            };
        
        Scanner::Token semiColon;

        std::string nodeName() { return "ReturnStmt: ";};

};

class BreakStmt : public ReturnStmt
{
    public:
        BreakStmt()
            : ReturnStmt()
            {

            };

        std::string nodeName() { return "BreakStmt: "; };
};

class WhileStmt : public KeywordStmt
{
    public:
        Scanner::Token lparen;
        Scanner::Token rparen;
        Statement *stmt;

        WhileStmt()
            : KeywordStmt()
            , lparen()
            , rparen()
            , stmt(nullptr)
        {

        };

        std::string nodeName() { return "WhileStmt: "; };
        std::string toString(int numSpaces);
};

class IfStmt : public WhileStmt
{
    public:
        Scanner::Token secondKeyword;
        Statement *elseBlock;

        IfStmt()
            : WhileStmt()
            , secondKeyword()
            , elseBlock(nullptr)
        {

        };

        std::string nodeName() { return "IfStmt: "; };
        std::string toString(int numSpaces);
};

class ForStmt : public WhileStmt
{
    public:
        Expression *startExpr;
        Scanner::Token endStart;
        Scanner::Token endExpr;
        Expression *loopExpr;

        ForStmt()
            : WhileStmt()
            , startExpr(nullptr)
            , endStart()
            , endExpr()
            , loopExpr(nullptr)
        {

        };

        std::string nodeName() { return "ForStmt: "; };
        std::string toString(int numSpace);
};

