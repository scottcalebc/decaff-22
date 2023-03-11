#include <vector>
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
        int line() { return 0; };
        std::string nodeName() { return "Stmt: ";};
        std::string toString(int numSpaces);
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
};

class Expression : public Statement
{
    protected:
        Expression()
            : left(nullptr)
        {

        };


    public:
        virtual ~Expression()
        {
            delete left;
        };
        Expression *left;
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
};



/**
 * Derived Classes for Actual Implementation / Derivation of Parse Tree
 */



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
};



class LValue : public Expression
{

    public:
        Identifier ident;

        LValue()
            : Expression()
            , ident()
        {

        };

        std::string nodeName() { return "LValue"; };
        std::string toString(int numSpaces);
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
};


class FormalVariableDeclaration : public VariableDeclaration
{
    public:
        FormalVariableDeclaration() 
            : VariableDeclaration()
        {

        };

        FormalVariableDeclaration(VariableDeclaration*var);
        std::string nodeName() { return "(formals):" + VariableDeclaration::nodeName(); };
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