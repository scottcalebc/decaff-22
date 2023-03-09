#include <vector>
#include "token.hpp"


class DeclarationType
{
    

    public:
        DeclarationType()
        {

        };

        Scanner::Token type;

        std::string toString(int numSpaces);
};


class Declarations
{
    protected:
        Declarations()
            : type(nullptr)
            , ident()
        {

        };

    public:
        DeclarationType *type;
        Scanner::Token ident;
        virtual ~Declarations() {};

        virtual int line() = 0;
        virtual std::string toString(int numSpaces) = 0;
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

        int line()
        {
            return ident.lineNumber;
        }
        std::string toString(int numSpaces);
};


class FormalVariableDeclaration : public VariableDeclaration
{
    public:
        FormalVariableDeclaration() 
            : VariableDeclaration()
        {

        };

        std::string toString(int numSpaces);
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

        int line()
        {
            return ident.lineNumber;
        };
        std::string toString(int numSpaces);
};