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

        int line()
        {
            return ident.lineNumber;
        }
        std::string nodeName();
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

        Scanner::Token type;

        int line()
        {
            return type.lineNumber;
        }
        std::string nodeName();
        std::string toString(int numSpaces);
};

class ReturnType: public DeclarationType
{
    public:
        ReturnType()
            : DeclarationType()
        {
            
        };
        std::string nodeName();
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

class Expression : public ParseNode
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

        std::string nodeName();
};


class FormalVariableDeclaration : public VariableDeclaration
{
    public:
        FormalVariableDeclaration() 
            : VariableDeclaration()
        {

        };

        std::string nodeName();
};

class FunctionDeclaration : public Declarations
{
    public:
        Scanner::Token lparen;         // Open Paren
        /*vector of formals since can be 0-inf (in theory) */
        std::vector<FormalVariableDeclaration*> formals;
        Scanner::Token rparen;
        //StmtBlock    block;

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


        std::string nodeName();
        std::string toString(int numSpaces);
};