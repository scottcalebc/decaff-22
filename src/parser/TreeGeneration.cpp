#include <deque>
#include <stack>
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

void eraseToken(Scanner::Token item)
{

    for(auto it = tokenLookAhead->begin(); it != tokenLookAhead->end();)
    {
        if (*it == item)
        {
            it = tokenLookAhead->erase(it);
            tokenLookAheadIndex--;
        } 
        else
        {
            ++it;
        }
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

void printStack(const char*name, std::stack<Scanner::Token> stack)
{
    std::cout << name << "Stack" << ": ";
    
    while (! stack.empty())
    {
        std::cout << stack.top().getValue<std::string>() << " ";
        stack.pop();
    }
    std::cout << std::endl;
}

Expression* parseExpr2(std::string sep)
{
    std::stack<Scanner::Token> tokenStack;
    
    Expression *expr = nullptr;

    // 
    std::stack<Scanner::Token> opHold;
    while ( tokenLookAhead->at(0).getValue<std::string>().compare(sep) != 0 
    && tokenLookAhead->at(0).type != Scanner::Token::Type::END )
    {
        // always make sure there are at least two tokens before parsing
        addLookAhead(std::abs(2 - (tokenLookAheadIndex+1) ) );
        
        switch(tokenLookAhead->at(0).type)
        {
            case Scanner::Token::Type::Identifier:
                if (tokenLookAhead->at(1).subType == Scanner::Token::SubType::Paren)
                {
                    tokenLookAhead->at(0).subType = Scanner::Token::SubType::Call;
                    // add call to op hold, need to parse rest of exprs which may have their own ops
                    opHold.push(tokenLookAhead->at(0));
                } else
                {
                    // add LValue
                    tokenStack.push(tokenLookAhead->at(0));
                }
                break;
            case Scanner::Token::Type::IntConstant :
            case Scanner::Token::Type::DoubleConstant : 
            case Scanner::Token::Type::BoolConstant :
            case Scanner::Token::Type::NullConstant : 
            case Scanner::Token::Type::StringConstant :
                // add constant 
                tokenStack.push(tokenLookAhead->at(0));
                break;
            case Scanner::Token::Type::Operator:
            case Scanner::Token::Type::Equal :
            case Scanner::Token::Type::NotEqual :
            case Scanner::Token::Type::GreaterEqual :
            case Scanner::Token::Type::LessEqual :
            case Scanner::Token::Type::And :
            case Scanner::Token::Type::Or :
                while (!opHold.empty() && opHold.top().subType > tokenLookAhead->at(0).subType)
                {
                    tokenStack.push(opHold.top());
                    opHold.pop();
                }
                opHold.push(tokenLookAhead->at(0));
                break;
            case Scanner::Token::Type::Separator :
                if (tokenLookAhead->at(0).subType == Scanner::Token::SubType::Paren
                && tokenLookAhead->at(0).getValue<std::string>().compare("(") == 0)
                {
                    // push opening paren onto token stack
                    tokenStack.push(tokenLookAhead->at(0));

                    // push into opHold to know how many operators are in a parenthesis section
                    opHold.push(tokenLookAhead->at(0));
                } else if (tokenLookAhead->at(0).subType == Scanner::Token::SubType::Paren
                || tokenLookAhead->at(0).subType == Scanner::Token::SubType::Comma)
                {
                    // dont take call token as we don't know if we are dealing with comma or paren
                    while(!opHold.empty() && (opHold.top().subType != Scanner::Token::SubType::Paren
                        && opHold.top().subType != Scanner::Token::SubType::Call) )
                    {
                        tokenStack.push(opHold.top());
                        opHold.pop();
                    }

                    if (!opHold.empty() )
                    {
                        // pop paren we don't need it anymore
                        if (opHold.top().subType == Scanner::Token::SubType::Paren)
                            opHold.pop();
                        
                        // if we finish call then we need to pull that off
                        if (tokenLookAhead->at(0).subType == Scanner::Token::SubType::Paren
                            && opHold.top().subType == Scanner::Token::SubType::Call)
                        {
                            tokenStack.push(opHold.top());
                            opHold.pop();
                        }
                    }
                }
                break;
            default:
                break;

        }

        // parsed token, need to take
        takeTokens(1);

    }
    
    if (tokenLookAhead->at(0).getValue<std::string>().compare(sep) != 0)
        throw std::runtime_error("Invalid expression");

    while (!opHold.empty())
    {
        tokenStack.push(opHold.top());
        opHold.pop();
    }

    printStack("Token", tokenStack);


    return new Expression();
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

        // Since relational/logical expr use custom token types operators will always be arithmetic
        ArithmeticExpression *binexpr = new ArithmeticExpression();
        binexpr->op = tokenLookAhead->at(1);
        
        eraseToken(binexpr->op);

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
        {
            LogicalExpression *expr = nullptr;
            switch(tokenLookAhead->at(1).type)
            {
                // equality statements (logical?)       ~
                case Scanner::Token::Type::Equal :
                case Scanner::Token::Type::NotEqual :
                case Scanner::Token::Type::GreaterEqual :
                case Scanner::Token::Type::LessEqual :
                case Scanner::Token::Type::And :
                case Scanner::Token::Type::Or :
                    expr = new LogicalExpression();
                    break;
                default:
                    break;
            }

            if (expr != nullptr)
            {
                // Found operator save top and take token
                Scanner::Token top = tokenLookAhead->at(0);
                expr->op = tokenLookAhead->at(1);
                eraseToken(expr->op);

                expr->expr = parseExpr();

                if (expr->expr == nullptr)
                {
                    throw std::runtime_error("Invalid Binary Expression with operator: " + expr->op.getValue<std::string>());
                }

                expr->right = parseExpr();

                if (expr->followExpr(dynamic_cast<LogicalExpression*>(expr->right)))
                    throw std::runtime_error("Invalid Binary expression with RHS: " + expr->right->toString(0));

                return expr;
            }
        }
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
            {
                Expression *expr = parseExpr2(";");

                if (tokenLookAhead->at(0).getValue<std::string>().compare(";") == 0)
                    takeTokens(1);
                else
                    throw std::runtime_error("Expected semicolon at end of expression");

                return expr;
            }
            break;    
        case Scanner::Token::Type::Separator:
            if (token.getValue<std::string>().compare("{") == 0)
                return parseStmtBlock();
            else    
                return nullptr;
        default:
            return parseExpr2(";");
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
                std::cout << "StmtBlock: " << "Got Statement: " << stmt->toString(0);
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

