#include <deque>
#include <iomanip>
#include <exception>

#include "TreeGeneration.hpp"
#include "lexer.hpp"
#include "exceptions.hpp"
#include "token.hpp"
#include "ParseTree.hpp"



// Forward Decls for Recursive calls
StatementBlock* parseStmtBlock();


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

void insertFront(Scanner::Token item)
{
    tokenLookAhead->push_front(item);

    // reset tokenLookAhead
    tokenLookAheadIndex = 0;
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

Expression* parseExpr()
{
    Scanner::Token token = tokenLookAhead->front();

    // need at least two tokens
    addLookAhead(std::abs(2 - (tokenLookAheadIndex+1) ) );

    if (tokenLookAhead->at(0).getValue<std::string>().compare("(") == 0)
    {
        std::cout << "Parsing ParenExpr" << std::endl << std::flush;
        ParenExpr *expr = new ParenExpr();

        expr->lparen = tokenLookAhead->at(0);
        takeTokens(1);

        expr->expr = parseExpr();

        if (expr->expr == nullptr)
            throw std::runtime_error("Invalid Expression");

        if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
            throw std::runtime_error("Missing ')' in Expression");
        
        expr->rparen = tokenLookAhead->at(0);
        takeTokens(1);

        return expr;
    }
    else if (tokenLookAhead->at(0).type == Scanner::Token::Type::Operator )
    {
        // unary operator

        std::cout << "Skipping unary op: " << tokenLookAhead->at(0);
        takeTokens(1);
        return new Expression();
    }
    else if (tokenLookAhead->at(1).type == Scanner::Token::Type::Operator )
    {
        // binary expression

        // needs to be either arithmetic or comparison/relational/logical
        BinaryExpression *binexpr = new BinaryExpression();
        binexpr->op = tokenLookAhead->at(1);

        // save token at front, pop two tokens, then push back
        Scanner::Token top = tokenLookAhead->front();
        takeTokens(2);
        insertFront(top);

        binexpr->expr = parseExpr();
        
        if (binexpr->expr == nullptr)
        {
            throw std::runtime_error("Invalid Binary Expression with operator: " + binexpr->op.getValue<std::string>());
        }

        binexpr->right = parseExpr();

        if (binexpr->right->nodeName().compare("Unary") == 0)
            throw std::runtime_error("Invalid Binary expression with RHS: " + binexpr->right->toString(0));

        return binexpr;
    } else if (tokenLookAhead->at(0).type == Scanner::Token::Type::Identifier 
        && tokenLookAhead->at(1).getValue<std::string>().compare("(") == 0 ) 
        // Call
    {
        if (tokenLookAhead->at(0).getValue<std::string>().compare("ReadInteger") == 0 
                || tokenLookAhead->at(0).getValue<std::string>().compare("ReadLine") == 0)
        {
            takeTokens(1);
            if (tokenLookAhead->at(0).getValue<std::string>().compare("(") != 0)
                throw std::runtime_error("Expected opening parenthesis");
            
            takeTokens(1);

            if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
                throw std::runtime_error("Expected closing parenthesis");

            takeTokens(1);
        }
    }
    else
    {
        switch (tokenLookAhead->at(0).type)
        {
            case Scanner::Token::Type::Identifier :
                {
                    LValue *expr = new LValue();
                    expr->ident = new Identifier();
                    expr->ident->ident = tokenLookAhead->at(0);

                    takeTokens(1);
                    return expr;
                }
            case Scanner::Token::Type::IntConstant :
            case Scanner::Token::Type::DoubleConstant : 
            case Scanner::Token::Type::BoolConstant :
            case Scanner::Token::Type::NullConstant : 
            case Scanner::Token::Type::StringConstant : 
                {
                    Constant *expr = new Constant();
                    expr->constant = tokenLookAhead->at(0);
                    takeTokens(1);

                    return expr;
                } 
                break;     
            default:
                return nullptr;
                break;
        }
    }


    while ( tokenLookAhead->at(0).getValue<std::string>().compare(";") != 0 && tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0
        && tokenLookAhead->at(0).type != Scanner::Token::Type::END )
    {
        std:: cout << "Expr: ";
        std::cout << "Skipping Token: " << tokenLookAhead->at(0) << std::endl;
        takeTokens(1);
    }


    return new Expression();

}

Statement* parseStmt()
{
    Scanner::Token token = tokenLookAhead->front();

    switch(token.type)
    {
        case Scanner::Token::Type::If:
        case Scanner::Token::Type::While:
        case Scanner::Token::Type::For:
        case Scanner::Token::Type::Break:
        case Scanner::Token::Type::Return:
            return nullptr;
            break;
        case Scanner::Token::Type::Identifier:
            if (token.getValue<std::string>().compare("Print") == 0)
                return nullptr;
            else
            {
                Expression *expr = parseExpr();

                if (tokenLookAhead->at(0).getValue<std::string>().compare(";") == 0)
                    takeTokens(1);
                else
                    throw std::runtime_error("Expected semicolon at end of expression");
            }
                
        case Scanner::Token::Type::Separator:
            if (token.getValue<std::string>().compare("{") == 0)
                return parseStmtBlock();
            else    
                return nullptr;
        default:
            return parseExpr();
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

            Statement *stmt = parseStmt();

            if (stmt == nullptr && tokenLookAhead->at(0).getValue<std::string>().compare("}") != 0)
            {
                // normally throw, for debug just skip token
                // throw std::runtime_error("Invalid token in statement");
                std::cout << "StmtBlock: ";
                std::cout << "Skipping Token: " << tokenLookAhead->at(0);
                takeTokens(1);
            }

            if (stmt != nullptr )
            {
                stmtBlock->stmts.push_back(stmt);
            }
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
        std::cout << "Decl: ";
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
                std::cout << "Prog: ";
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

