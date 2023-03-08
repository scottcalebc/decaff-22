
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
        {

        };

    public:
        DeclarationType *type;
        Scanner::Token ident;
        virtual ~Declarations() {};
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
        DeclarationType *type;
        Scanner::Token ident;
        Scanner::Token semiColon;

        VariableDeclaration() : Declarations()
        {

        };

        std::string toString(int numSpaces);
};