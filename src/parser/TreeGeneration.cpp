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




VariableDeclaration* parseVarDecl()
{
    Scanner::Token token = tokenLookAhead->front();

    if (tokenLookAheadIndex + 1 < 3)
    {
        addLookAhead(std::abs(3 - (tokenLookAheadIndex+1) ) );
    }

    // Variable Decl (we already checked Type to get here)
    switch(token.type)
    {
        case Scanner::Token::Type::Int:
        case Scanner::Token::Type::Bool:
        case Scanner::Token::Type::Double:
        case Scanner::Token::Type::String:
            // Must be: Type Ident ;
            if (
                tokenLookAhead->at(1).type == Scanner::Token::Type::Identifier &&
                tokenLookAhead->at(2).type == Scanner::Token::Type::Separator
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

                return var;
            }
            break;
        default:
            return nullptr;
    }

    return nullptr;
}

StatementBlock* parseStmtBlock()
{

    Scanner::Token token = tokenLookAhead->front();

    if (token.type != Scanner::Token::Type::Separator &&
        token.getValue<std::string>().compare("{") != 0)
    {
        throw std::runtime_error("Expected opening brace to statement body");
    }

    StatementBlock * stmtBlock = new StatementBlock();
    stmtBlock->lbrace = token;

    takeTokens(1);

    // While we haven't reached the end
    if (tokenLookAhead->at(0).getValue<std::string>().compare("}") != 0 )
    {
        // parse var decls
        VariableDeclaration *varDecl = nullptr;
        do {
            // will add to lookup as necessary
            varDecl = parseVarDecl();

            if (varDecl != nullptr)
            {
                if (varDecl->semiColon.getValue<std::string>().compare(";") != 0)
                {
                    throw std::runtime_error("Error expected semicolon to end statemnt");
                }
                takeTokens(3); // take varDecl tokens
                stmtBlock->vars.push_back(varDecl);
            }
        } while(varDecl != nullptr);

        // parse statements
        while (tokenLookAhead->at(0).getValue<std::string>().compare("}") != 0)
        {
            // extra tokens
            std::cout << "Skipping token: " << tokenLookAhead->at(0) << std::endl << std::flush;
            takeTokens(1);
        }

    }

    if (tokenLookAhead->at(0).getValue<std::string>().compare("}") == 0)
    {
        stmtBlock->rbrace = tokenLookAhead->at(0);
        takeTokens(1);
        return stmtBlock;
    }


    return nullptr;
}

std::vector<FormalVariableDeclaration*> parseFormals()
{
    std::vector<FormalVariableDeclaration*> formals;

    // If we haven't hit the end of the formals keep going
    while(tokenLookAhead->front().getValue<std::string>().compare(")") != 0)
    {
        // Here we need up to 3 tokens to determine which direction to go
        addLookAhead(std::abs(3 - (tokenLookAheadIndex+1) ) );
        if (
        (tokenLookAhead->at(2).getValue<std::string>().compare(",") == 0
                || tokenLookAhead->at(2).getValue<std::string>().compare(")") == 0)
        )
        {
            VariableDeclaration *var = parseVarDecl();            
            FormalVariableDeclaration *formalVar = new FormalVariableDeclaration(var);

            if (var->semiColon.getValue<std::string>().compare(",") == 0)
            {
                formalVar->semiColon = var->semiColon; 
                takeTokens(3);
            }
            else
            {
                formalVar->semiColon.type = Scanner::Token::Type::EMPTY;
                takeTokens(2);
            }

            delete var;
            formals.push_back(formalVar);

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

    Declarations* decl = nullptr;
    if ( tokenLookAhead->at(2).getValue<std::string>().compare(";") == 0)
    {
        decl = parseVarDecl();
        takeTokens(3);  // take semi

        return decl;
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
        func->block = parseStmtBlock();

        if (func->block == nullptr)
            throw std::runtime_error("Expected Closing brace to statement block");

        return func;
    }
    else
    {
        // For testing we Are just going to destroy DeclType and eat the front token
        std::cout << "Skipping Token: " << token;
        takeTokens(1);

        return nullptr;
    }

    return decl;
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

