#include <deque>
#include <stack>
#include <iomanip>
#include <exception>

// custom library includes
#include <lexer/lexer.hpp>
#include <lexer/exceptions.hpp>
#include <token/token.hpp>

// this libraries includes
#include "TreeGeneration.hpp"
#include "ParseTree.hpp"
#include "exceptions.hpp"

namespace Parser {
// Forward Decls for Recursive calls
StatementBlock* parseStmtBlock();
Statement* parseStmt();


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
            else if (tokenLookAhead->at(1).type != Scanner::Token::Type::Identifier)
                throw Parser::ParseException(tokenLookAhead->at(1));
            else
                throw Parser::ParseException(tokenLookAhead->at(2));
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

std::stack<Scanner::Token> infix2postfix(std::string sep)
{
    std::stack<Scanner::Token> tokenStack;
    // 
    std::stack<Scanner::Token> opHold;

    // keep last token evaluated, for finding unary operators
    Scanner::Token last;
    while ( tokenLookAhead->at(0).getValue<std::string>().compare(sep) != 0 
    && tokenLookAhead->at(0).type != Scanner::Token::Type::END )
    {
        // always make sure there are at least two tokens before parsing
        addLookAhead(std::abs(2 - (tokenLookAheadIndex+1) ) );
        
        switch(tokenLookAhead->at(0).type)
        {
            case Scanner::Token::Type::Identifier:
                if (tokenLookAhead->at(1).subType == Scanner::Token::SubType::Paren
                    && tokenLookAhead->at(1).getValue<std::string>().compare("(") == 0)
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
                if (tokenLookAhead->at(0).subType == Scanner::Token::SubType::Subtract 
                && (
                    tokenStack.empty() ||
                    last.subType != Scanner::Token::SubType::Operand 
                ))
                {
                    tokenLookAhead->at(0).subType = Scanner::Token::SubType::UnaryNegative;
                }
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
                    // push all operators that might be holding between paren or commas to make sure
                    //  expr in parenthesis and arguments for functions are evaluated correctly 
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
                        if (tokenLookAhead->at(0).subType == Scanner::Token::SubType::Paren)
                        {
                            tokenStack.push(tokenLookAhead->at(0));
                            if (!opHold.empty() && opHold.top().subType == Scanner::Token::SubType::Call)
                            {
                                tokenStack.push(opHold.top());
                                opHold.pop();
                            }
                        }
                    }
                } else {
                    addLookAhead(2);
                    if (tokenLookAhead->size() < 2)
                        throw Parser::ParseException(tokenLookAhead->at(0));
                    else
                        throw Parser::ParseException(tokenLookAhead->at(1));
                }
                break;
            default:
                // unexpected token here 
                throw Parser::ParseException(tokenLookAhead->at(0));
                break;

        }

        // last token parsed
        last = tokenLookAhead->at(0);
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

    return tokenStack;
}

Expression* parseExpr(std::stack<Scanner::Token> &tokenStack)
{
    if (tokenStack.empty())
        return nullptr;


    switch (tokenStack.top().subType)
    {
    case Scanner::Token::SubType::Operand :
        if (tokenStack.top().type == Scanner::Token::Type::Identifier )
        {
            LValue *lvalue = new LValue();
            lvalue->ident = new Identifier();
            lvalue->ident->ident = tokenStack.top();

            tokenStack.pop();

            return lvalue;
        } else {
            Constant *constant = new Constant();
            constant->constant = tokenStack.top();

            tokenStack.pop();

            return constant;
        }
        break;
    case Scanner::Token::SubType::Paren:
        if (tokenStack.top().getValue<std::string>().compare(")") == 0)
        {
            ParenExpr *paren = new ParenExpr();
            paren->rparen = tokenStack.top();

            tokenStack.pop();
            paren->expr = parseExpr(tokenStack);

            if (tokenStack.top().getValue<std::string>().compare("(") != 0)
                throw Parser::ParseException(tokenStack.top());
            
            paren->lparen = tokenStack.top();
            tokenStack.pop();

            return paren;
        } else
        {
            return nullptr;
        }
        break;
    case Scanner::Token::SubType::Assign:
        {
            AssignExpression *assign = new AssignExpression();

            assign->op = tokenStack.top();

            tokenStack.pop();
            assign->right = parseExpr(tokenStack);

            if (assign->right == nullptr)
                throw Parser::ParseException(assign->op);
            
            assign->expr = parseExpr(tokenStack);

            if (assign->expr == nullptr)
                throw Parser::ParseException(assign->op);

            if (dynamic_cast<LValue*>(assign->expr) == nullptr)
                throw Parser::ParseException(assign->expr->firstToken());

            return assign;
        }
    case Scanner::Token::SubType::Add :
    case Scanner::Token::SubType::Subtract :
    case Scanner::Token::SubType::Divide :
    case Scanner::Token::SubType::Multiply :
    case Scanner::Token::SubType::Modulus :
        {
            ArithmeticExpression *math = new ArithmeticExpression();
            math->op = tokenStack.top();

            tokenStack.pop();

            Scanner::Token t;
            if (!tokenStack.empty())
                t = tokenStack.top();

            math->right = parseExpr(tokenStack);
            
            if (math->right == nullptr)
                throw Parser::ParseException(math->op);

            Scanner::Token t1;
            if (!tokenStack.empty())
                t = tokenStack.top();

            math->expr = parseExpr(tokenStack);

            if (math->expr == nullptr)
                throw Parser::ParseException(math->op);
            
            return math;
        }
        break;    
    case Scanner::Token::SubType::LessThan :
    case Scanner::Token::SubType::LessEqual :
    case Scanner::Token::SubType::GreaterThan :
    case Scanner::Token::SubType::GreaterEqual :
    case Scanner::Token::SubType::Equal :
    case Scanner::Token::SubType::NotEqual :
    case Scanner::Token::SubType::And :
    case Scanner::Token::SubType::Or :
        {
            LogicalExpression *logic;

            Scanner::Token op = tokenStack.top();
            if (op.subType == Scanner::Token::SubType::And || op.subType == Scanner::Token::SubType::Or)
                logic = new LogicalExpression();
            else if (op.subType == Scanner::Token::SubType::Equal || op.subType == Scanner::Token::SubType::NotEqual)
                logic = new EqualityExpression();
            else
                logic = new RelationalExpression();
            
            
            logic->op = op;

            tokenStack.pop();
            logic->right = parseExpr(tokenStack);

            // Probably easier way to check
            // This checks if the right expressions is a logical expression cannot have something like
            // a > b > c (not parsable)
            // a > b && a > c (parsable)
            if (logic->right == nullptr)
                throw Parser::ParseException(logic->op);

            if (logic->op.subType != Scanner::Token::SubType::And && logic->op.subType != Scanner::Token::SubType::Or &&
                dynamic_cast<BinaryExpression*>(logic->right) != nullptr && (
                    dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::LessThan
                ||  dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::LessEqual
                ||  dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::GreaterThan
                ||  dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::GreaterEqual
                ||  dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::Equal
                ||  dynamic_cast<BinaryExpression*>(logic->right)->op.subType  == Scanner::Token::SubType::NotEqual) )
            {
                throw Parser::ParseException(dynamic_cast<BinaryExpression*>(logic->right)->op);
            }

            logic->expr = parseExpr(tokenStack);

            if (logic->right == nullptr)
                throw Parser::ParseException(logic->op);
            
            if (logic->op.subType != Scanner::Token::SubType::And && logic->op.subType != Scanner::Token::SubType::Or &&
                dynamic_cast<BinaryExpression*>(logic->expr) != nullptr 
                && (   dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::LessThan
                    || dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::GreaterThan
                    || dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::GreaterEqual
                    || dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::Equal
                    || dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::LessEqual
                    || dynamic_cast<BinaryExpression*>(logic->expr)->op.subType  == Scanner::Token::SubType::NotEqual ) )
            {
                throw Parser::ParseException(dynamic_cast<BinaryExpression*>(logic->right)->op);
            }

            return logic;
        }

    case Scanner::Token::SubType::Not :
    case Scanner::Token::SubType::UnaryNegative :
        {
            UnaryExpression *unary = new UnaryExpression;
            unary->op = tokenStack.top();

            tokenStack.pop();
            unary->expr = parseExpr(tokenStack);

            if (unary->expr == nullptr)
                throw Parser::ParseException(unary->op);

            return unary;
        }

    case Scanner::Token::SubType::Call:
        {
            CallExpression *call;

            if (tokenStack.top().getValue<std::string>().compare("Print") == 0)
            {
                call = new PrintStmt();
            }
            else if (tokenStack.top().getValue<std::string>().compare("ReadInteger") == 0)
            {
                call = new ReadIntExpr();
            }
            else if (tokenStack.top().getValue<std::string>().compare("ReadLine") == 0)
            {
                call = new ReadLineExpr();
            }
            else
            {
                call = new CallExpression();
            }

            call->ident = new Identifier();
            call->ident->ident = tokenStack.top();
            tokenStack.pop();

            if (tokenStack.top().subType != Scanner::Token::SubType::Paren )
                throw Parser::ParseException(tokenStack.top());
            
            call->rparen = tokenStack.top();
            tokenStack.pop();

            while(!tokenStack.empty() && tokenStack.top().subType != Scanner::Token::SubType::Paren )
            {
                call->actuals.push_front(parseExpr(tokenStack));
            }

            if (tokenStack.top().subType != Scanner::Token::SubType::Paren )
                throw Parser::ParseException(tokenStack.top());
            
            call->lparen = tokenStack.top();
            tokenStack.pop();


            if (call->ident->ident.getValue<std::string>().compare("Print") == 0 
                && call->actuals.size() == 0 )
            {
                throw Parser::ParseException(call->lparen);
            }
            if ( (call->ident->ident.getValue<std::string>().compare("ReadInteger") == 0 
                || call->ident->ident.getValue<std::string>().compare("ReadLine") == 0 )
                && call->actuals.size() != 0 )
            {
                throw Parser::ParseException(call->lparen);
            }

            // return empty expression
            return call;
        }
        break;
    default:
        break;
    }


    return nullptr;
}

WhileStmt * parseWhile()
{
    WhileStmt *stmt = new WhileStmt();

    stmt->keyword = tokenLookAhead->at(0);
    takeTokens(1);

    if (tokenLookAhead->at(0).getValue<std::string>().compare("(") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));

    stmt->lparen = tokenLookAhead->at(0);
    takeTokens(1);

    // parse expr up until close paren
    std::stack<Scanner::Token> tokens = infix2postfix(")");
    stmt->expr = parseExpr(tokens);

    if (stmt->expr == nullptr)
        throw std::runtime_error("Invalid expression for while");

    if (!tokens.empty())
        throw Parser::ParseException(stmt->expr->firstToken());

    if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));

    stmt->rparen = tokenLookAhead->at(0);
    takeTokens(1);

    // parse statement(s)
    stmt->stmt = parseStmt();

    if (stmt->stmt == nullptr)
        throw std::runtime_error("Expected statement for while");

    return stmt;
}

IfStmt* parseIfStmt()
{
    IfStmt *stmt = new IfStmt();

    stmt->keyword = tokenLookAhead->at(0);
    takeTokens(1);

    if (tokenLookAhead->at(0).getValue<std::string>().compare("(") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));

    stmt->lparen = tokenLookAhead->at(0);
    takeTokens(1);

    // parse expr up until close paren
    std::stack<Scanner::Token> tokens = infix2postfix(")");
    stmt->expr = parseExpr(tokens);

    if (stmt->expr == nullptr)
        throw std::runtime_error("Invalid expression for If");

    if (!tokens.empty())
        throw Parser::ParseException(stmt->expr->firstToken());

    if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));

    
    stmt->rparen = tokenLookAhead->at(0);
    takeTokens(1);

    // parse statement(s)
    stmt->stmt = parseStmt();

    if (stmt->stmt == nullptr)
        throw std::runtime_error("Expected statement for If");


    if (tokenLookAhead->at(0).type == Scanner::Token::Type::Else)
    {
        stmt->secondKeyword = tokenLookAhead->at(0);
        takeTokens(1);

        stmt->elseBlock = parseStmt();

        if (stmt->elseBlock == nullptr)
            throw std::runtime_error("Expected statement following else keyword");
        
    }
    
    return stmt;
}

ForStmt* parseFor()
{
    ForStmt *stmt = new ForStmt();

    stmt->keyword = tokenLookAhead->at(0);
    takeTokens(1);

    if (tokenLookAhead->at(0).getValue<std::string>().compare("(") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));

    stmt->lparen = tokenLookAhead->at(0);
    takeTokens(1);

    if (tokenLookAhead->at(0).getValue<std::string>().compare(";") != 0)
    {
        std::stack<Scanner::Token> tokens = infix2postfix(";");
        int numTokens = tokens.size();
        stmt->startExpr = parseExpr(tokens);

        if (stmt->startExpr == nullptr)
            throw std::runtime_error("Expected expression");
        
        if (!tokens.empty())
            throw Parser::ParseException(stmt->expr->firstToken());

        // check again after parsing to verify
        if (tokenLookAhead->at(0).getValue<std::string>().compare(";") != 0)
            throw Parser::ParseException(tokenLookAhead->at(0));
    } 
    
    stmt->endStart = tokenLookAhead->at(0);
    takeTokens(1);

    std::stack<Scanner::Token> tokens = infix2postfix(";");
    stmt->expr = parseExpr(tokens);

    if (stmt->expr == nullptr)
        throw std::runtime_error("Invalid Expression");

    if (!tokens.empty())
        throw Parser::ParseException(stmt->expr->firstToken());

    if (tokenLookAhead->at(0).getValue<std::string>().compare(";") != 0)
        throw Parser::ParseException(tokenLookAhead->at(0));
    
    stmt->endExpr = tokenLookAhead->at(0);
    takeTokens(1);

    if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
    {
        std::stack<Scanner::Token> tokens = infix2postfix(")");
        stmt->loopExpr = parseExpr(tokens);

        if (stmt->loopExpr == nullptr)
            throw std::runtime_error("Expected expression");
        
        if (!tokens.empty())
            throw Parser::ParseException(stmt->expr->firstToken()); 
        
        if (tokenLookAhead->at(0).getValue<std::string>().compare(")") != 0)
            throw Parser::ParseException(tokenLookAhead->at(0));
    }

    stmt->rparen = tokenLookAhead->at(0);
    takeTokens(1);

    stmt->stmt = parseStmt();

    if (stmt->stmt == nullptr)
        throw std::runtime_error("Expected statement");

    return stmt;
}

Statement* parseStmt()
{
    Scanner::Token token = tokenLookAhead->front();

    switch(token.type)
    {
        case Scanner::Token::Type::If:
            return parseIfStmt();
        case Scanner::Token::Type::While:
            return parseWhile();
            break;
        case Scanner::Token::Type::For:
            return parseFor();
            break;
        case Scanner::Token::Type::Break:
            {
                BreakStmt *breakStmt = new BreakStmt();

                breakStmt->keyword = tokenLookAhead->at(0);
                takeTokens(1);

                if (tokenLookAhead->at(0).getValue<std::string>().compare(";") != 0)
                {
                    throw std::runtime_error("Expected semicolon for break statement");
                }

                breakStmt->semiColon = tokenLookAhead->at(0);
                takeTokens(1);

                return breakStmt;
            }
            break;
        case Scanner::Token::Type::Return:
            {
                ReturnStmt *ret = new ReturnStmt();
                ret->keyword = tokenLookAhead->at(0);
                takeTokens(1);
                std::stack<Scanner::Token> tokens = infix2postfix(";");
                ret->expr = parseExpr(tokens);

                if (!tokens.empty() && ret->expr != nullptr)
                    throw Parser::ParseException(ret->expr->firstToken());

                if (!tokens.empty())
                    throw Parser::ParseException(tokenLookAhead->at(0));

                if (tokenLookAhead->at(0).getValue<std::string>().compare(";") == 0)
                {
                    ret->semiColon = tokenLookAhead->at(0);

                    // expr optional
                    if (ret->expr != nullptr)
                        ret->expr->semiColon = tokenLookAhead->at(0);

                    takeTokens(1);
                }
                else
                    throw Parser::ParseException(tokenLookAhead->at(0));

                return ret;
            }
            break;
        case Scanner::Token::Type::Separator:
            if (token.getValue<std::string>().compare("{") == 0)
                return parseStmtBlock();
            else    
                return nullptr;
        case Scanner::Token::Type::Identifier:
            {
                auto op = Scanner::Token::keywords.find(token.getValue<std::string>());
                if (op != Scanner::Token::keywords.end())
                    throw Parser::ParseException(token);
            }
        default:
            {
                std::stack<Scanner::Token> tokens = infix2postfix(";");
                Expression *expr = parseExpr(tokens);

                // todo: add this logic to all expression parsing
                if (!tokens.empty())
                    throw Parser::ParseException(expr->firstToken());

                if (tokenLookAhead->at(0).getValue<std::string>().compare(";") == 0)
                {
                    expr->semiColon = tokenLookAhead->at(0);
                    takeTokens(1);
                }
                else
                    throw std::runtime_error("Expected semicolon at end of expression");

                return expr;
            }
            break;
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
        VariableDeclaration *var = parseVarDecl();        

        if (var == nullptr)
            throw Parser::ParseException(tokenLookAhead->at(0));    
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


        if (decl == nullptr)
            throw Parser::ParseException(tokenLookAhead->at(0));
        
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

Program* parseProgram()
{
    // Program Decl Tree node
    // ParseTree root;

    Program *program = new Program();

    while(tokenLookAhead->front().type != Scanner::Token::Type::END )
    {
        Scanner::Token token = tokenLookAhead->front();

        switch(token.type)
        {
            case Scanner::Token::Type::Int:
            case Scanner::Token::Type::Void:
            case Scanner::Token::Type::Bool:
            case Scanner::Token::Type::Double:
            case Scanner::Token::Type::String:
                {
                    Declarations* decl = parseDecl();
                    if (decl == nullptr)
                        throw Parser::ParseException(tokenLookAhead->at(0));

                    program->decls.push_back(decl);
                }    
                break;
            default:
                throw Parser::ParseException(tokenLookAhead->at(0));

        }
    }

    return program;
}


    Program* treeGeneration(Scanner::Lexer *lexer, bool print)
    {
        // Setup look ahead if necessary for certain parsing calls
        if (Parser::tokenLookAhead == nullptr)
        {
            Parser::tokenLookAhead = new std::deque<Scanner::Token>();
        }
        
        Parser::glexer = lexer;
        try
        {
            Parser::addLookAhead();
            Parser::Program* p = Parser::parseProgram();

            if (print)
                std::cout << std::endl << p->toString(0);

            return p;
        }
        catch( Scanner::GenericException &exc )
        {
            std::cout << exc.what() << std::endl;
        }

        return nullptr;
    }
}


