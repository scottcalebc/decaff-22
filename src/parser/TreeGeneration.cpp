#include <deque>
#include <iomanip>
#include <exception>

#include "TreeGeneration.hpp"
#include "lexer.hpp"
#include "exceptions.hpp"
#include "token.hpp"
#include "ParseTree.hpp"


Scanner::Lexer             *glexer;
std::deque<Scanner::Token> *tokenLookAhead;
int tokenLookAheadIndex = -1;


void addLookAhead(int numLookAheads = 1)
{
    for (int i = 0; i < numLookAheads; i++)
    {
        tokenLookAhead->push_back(glexer->getNextToken());
        tokenLookAheadIndex++;
    }
}

void takeTokens(int numTokens)
{
    if (numTokens > tokenLookAheadIndex + 1 )
        throw std::runtime_error("Cannot take more tokens than what's in container");

    for (int i = 0; i < numTokens; i++)
    {
        tokenLookAhead->pop_front();
        tokenLookAheadIndex--;
    }

    // If we have eaten all the lookahead, then replace
    if (tokenLookAheadIndex == -1)
        addLookAhead();
}

std::vector<FormalVariableDeclaration*> parseFormals()
{
    std::vector<FormalVariableDeclaration*> formals;

    // If we haven't hit the end of the formals keep going
    while(tokenLookAhead->front().getValue<std::string>().compare(")") != 0)
    {
        // Here we need up to 3 tokens to determine which direction to go
        addLookAhead(std::abs(3 - (tokenLookAheadIndex+1) ) );

        Scanner::Token token = tokenLookAhead->front();

        DeclarationType *type = new DeclarationType();
        type->type = token;

        switch(token.type)
        {
            case Scanner::Token::Type::Int:
            case Scanner::Token::Type::Void:
            case Scanner::Token::Type::Bool:
            case Scanner::Token::Type::Double:
            case Scanner::Token::Type::String:
                if (
                    tokenLookAhead->at(0).type != Scanner::Token::Type::Void &&
                    tokenLookAhead->at(1).type == Scanner::Token::Type::Identifier &&
                    tokenLookAhead->at(2).type == Scanner::Token::Type::Separator &&
                    
                    (tokenLookAhead->at(2).getValue<std::string>().compare(",") == 0
                        || tokenLookAhead->at(2).getValue<std::string>().compare(")") == 0)
                )
                {
                    Identifier *ident = new Identifier();
                    ident->ident = tokenLookAhead->at(1);

                    FormalVariableDeclaration *var = new FormalVariableDeclaration();
                    var->type = type;
                    var->ident = ident;

                    if (tokenLookAhead->at(2).getValue<std::string>().compare(",") == 0)
                    {
                        var->semiColon = tokenLookAhead->at(2);
                        takeTokens(3);
                    }
                    else
                    {
                        var->semiColon.type = Scanner::Token::Type::EMPTY;
                        takeTokens(2);
                    }

                    formals.push_back(var);

                } 
                break;
            default:
                delete type;
                // print error
                std::cout << "Skipping Token: " << token;
                takeTokens(1);

        }

 
    }
    // leave rparen in token feed to be eaten by func decl

    return formals;
}

Declarations* parseDecl()
{
    // Decide what type of decl 
    
    // useful hold for error printing of current token in lookahead
    Scanner::Token token = tokenLookAhead->front();

    // Here we need up to 3 tokens to determine which direction to go
    addLookAhead(std::abs(3 - (tokenLookAheadIndex+1) ) );

    // Variable Decl (we already checked Type to get here)
    // Must be: Type Ident ;
    if (
        tokenLookAhead->at(0).type != Scanner::Token::Type::Void &&
        tokenLookAhead->at(1).type == Scanner::Token::Type::Identifier &&
        tokenLookAhead->at(2).type == Scanner::Token::Type::Separator &&
        tokenLookAhead->at(2).getValue<std::string>().compare(";") == 0
    )
    {
        // Type Node
        DeclarationType *type = new DeclarationType();
        type->type = token;

        // Ident Node
        Identifier *ident = new Identifier();
        ident->ident = tokenLookAhead->at(1);

        // Variable node
        VariableDeclaration *var = new VariableDeclaration();
        var->type = type;
        var->ident = ident;
        var->semiColon = tokenLookAhead->at(2);

        takeTokens(3);
        return var;
    } 
    else if (
        tokenLookAhead->at(1).type == Scanner::Token::Type::Identifier &&
        tokenLookAhead->at(2).type == Scanner::Token::Type::Separator &&
        tokenLookAhead->at(2).getValue<std::string>().compare("(") == 0
    )
    {
        ReturnType *type = new ReturnType();
        Identifier *ident = new Identifier();
        FunctionDeclaration *func = new FunctionDeclaration();

        // setup return type node
        type->type = token;

        // setup ident
        ident->ident = tokenLookAhead->at(1);
        
        
        func->type = type;
        func->ident = ident;
        func->lparen = tokenLookAhead->at(2);

        

        takeTokens(3);

        // should add tokens to lookup until see a rparen, for parsing validity
        // before attempting to even verify formals are correct, but this is right 'recursive'

        // Take tokens up to lparen, next lookahead should be start of formals
        func->formals = parseFormals();
        func->rparen = tokenLookAhead->front();
        // take rparen
        takeTokens(1);
        //func->block = parseStmtBlock();

        return func;
    }
    else
    {
        // For testing we Are just going to destroy DeclType and eat the front token
        std::cout << "Skipping Token: " << token;
        takeTokens(1);

        return nullptr;
    }
}

void parseProgram()
{
    // Program Decl Tree node
    // ParseTree root;

    while(tokenLookAhead->front().type != Scanner::Token::Type::END )
    {
        Scanner::Token token = tokenLookAhead->front();

        Declarations* decl;

        switch(token.type)
        {
            case Scanner::Token::Type::Int:
            case Scanner::Token::Type::Void:
            case Scanner::Token::Type::Bool:
            case Scanner::Token::Type::Double:
            case Scanner::Token::Type::String:
                decl = parseDecl();
                break;
            default:
                // print error
                std::cout << "Skipping Token: " << token;
                takeTokens(1);

        }

        if (decl != nullptr)
        {
            std::cout << std::setw(3) << decl->line() 
                        << std::setw(3) << " " 
                        << decl->toString(6);
        }
        decl = nullptr;
    }
}

void treeGeneration(Scanner::Lexer *lexer)
{
    // Setup look ahead if necessary for certain parsing calls
    if (tokenLookAhead == nullptr)
    {
        tokenLookAhead = new std::deque<Scanner::Token>();
    }
    
    glexer = lexer;
    try
    {
        addLookAhead();
        parseProgram();
    }
    catch( Scanner::GenericException &exc )
    {
        std::cout << exc.what() << std::endl;
    }
}

