#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>

#include "STTypeVisitor.hpp"


namespace SemanticAnalyzer {

    bool typeCheck(AST::Program *p)
    {
        STTypeVisitor visitor;
        p->accept(&visitor);

        // returns true if type check passed
        return ! visitor.err;
    }

    void STTypeVisitor::printTypeError(Scanner::Token token, std::string errStr)
    {
        printTypeError(token.colStart - 1, token.getValue<std::string>().length(), 
                        token.lineNumber, token.lineInfo, errStr);
    }

    void STTypeVisitor::printTypeError(AST::Node* p, int lineNumber, std::string lineInfo, std::string errStr)
    {
        printTypeError(p->minCol() - 1, p->maxCol() - p->minCol(), lineNumber, lineInfo, errStr);
    }

    void STTypeVisitor::printTypeError(int start, int length, int lineNumber, std::string lineInfo, std::string errStr)
    {
        err = true;
        std::cout   << std::endl
                    << "*** Error line " << lineNumber << ".\n"
                    <<  lineInfo << std::endl
                    << std::setw(start) << " "
                    << std::setfill('^') << std::setw(length) << "" << std::endl
                    << "*** " << errStr << std::endl
                    << std::endl;

        std::cout << std::setfill(' ');
    }


    bool STTypeVisitor::arithmeticCheck(Scanner::Token::Token::Type type)
    {
        switch(type)
        {
            case Scanner::Token::Type::Int:
            case Scanner::Token::Type::Double:
                return true;
            default:
                return false;

        }
    }

    bool STTypeVisitor::binaryTypeCheck(AST::Node *left, AST::Node *right, bool unary)
    {
        Scanner::Token::Type ltype;
        Scanner::Token::Type rtype;
        if (left != nullptr)
        {
            left->accept(this);
            ltype = left->outType;

            if ( ltype == Scanner::Token::Type::ERROR )
            {   
                std::cout << "Error getting type from left expression\n";
                return false;
            }

            if (right != nullptr && ! unary)
            {
                right->accept(this);
                rtype = right->outType;


                if ( ltype == rtype )
                {
                    return true;
                }
            }
            else if ( ! unary )
            {
                std::cout << "Cannot get right type from empty expression\n";
            }
            else 
            {
                // we hit a unary so return true
                return true;
            }
        }
        else
        {
            std::cout << "Cannot get left type from empty expression\n";
        }

        return false;
    }

    void STTypeVisitor::visit(AST::Ident *p)
    {
        int space(0);
        // need to check if symbol is in table
        // std::cout << "Checking for symbol in table: " 
        //     << p->pScope->toString(space);
        SymbolTable::IdEntry *entry = p->pScope->idLookup(p->value.getValue<std::string>());

        if (entry == nullptr || entry->func)
        {
            exprErr = true;
            p->pScope->fakeInstall(p->value.getValue<std::string>());
            std::stringstream ss;
            ss << "No declaration found for variable '" << p->value.getValue<std::string>() << "'";
            printTypeError(p->value, ss.str());
        }

        // need to handle if id doesn't exist in table;
        p->outType = entry->type;
    }

    void STTypeVisitor::visit(AST::Constant *p)
    {
        switch(p->value.type)
        {
            case Scanner::Token::Type::IntConstant:
                p->outType = Scanner::Token::Type::Int;
                break;
            case Scanner::Token::Type::DoubleConstant:
                p->outType = Scanner::Token::Type::Double;
                break;
            case Scanner::Token::Type::BoolConstant:
                p->outType = Scanner::Token::Type::Bool;
                break;
            case Scanner::Token::Type::StringConstant:
                p->outType = Scanner::Token::Type::String;
                break;
            case Scanner::Token::Type::NullConstant:
                p->outType = Scanner::Token::Type::Void;
                break;
            default:
                p->outType = p->value.type;
        }
    } 

    void STTypeVisitor::visit(AST::LessThan *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::GreaterThan *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::GTE *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::LTE *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::NotEqual *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Not *p)
    {
        if ( binaryTypeCheck(p->left, p->right, true) && p->left->outType == Scanner::Token::Type::Bool )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);

            std::stringstream ss;
            ss << "Incompatible operand: " << p->op.getValue<std::string>() << " " << ltype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Equal *p)
    {
        if ( binaryTypeCheck(p->left, p->right) )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Or *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && p->left->outType == Scanner::Token::Type::Bool )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::And *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && p->left->outType == Scanner::Token::Type::Bool )
        {
            p->outType = Scanner::Token::Type::Bool;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Modulus *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Divide *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Multiply *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Subtract *p)
    {
        bool unary = p->right == nullptr;
        if ( binaryTypeCheck(p->left, p->right, unary) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else if (! exprErr) {
            exprErr = true;
            // TODO may need to add check for unary
            std::stringstream ss;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            if (! unary)
            {
                std::string rtype( Scanner::Token::getTypeName(p->right->outType) );
                std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
                ss << "Incompatible operands: " << ltype
                    << " " << p->op.getValue<std::string>() << " " << rtype;
                printTypeError(p->op, ss.str());
            } else 
            {
                ss << "Incompatible operand: "<< p->op.getValue<std::string>() << " " << ltype;
                printTypeError(p->op, ss.str());
            }
        }
    }

    void STTypeVisitor::visit(AST::Add *p)
    {
        if ( binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else if (! exprErr){
            exprErr = true;
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::Assign *p)
    {

        if ( binaryTypeCheck(p->left, p->right) )
        {
            // Do I need an out type on assign? Probably not, cannot have multiple assignments
            // p->outType = p->left->outType;
        } else if (! exprErr){
            std::string ltype( Scanner::Token::getTypeName(p->left->outType) );
            std::string rtype( Scanner::Token::getTypeName(p->right->outType) );

            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
            std::stringstream ss;
            ss << "Incompatible operands: " << ltype
                << " " << p->op.getValue<std::string>() << " " << rtype;
            printTypeError(p->op, ss.str());
        }

    }

    void STTypeVisitor::visit(AST::StatementBlock *p)
    {
        bool prevLoop = inLoop;
        for(auto &stmt : p->stmts)
        {
            exprErr = false;    // set expr err to false to ensure correct error printing
            stmt->accept(this);
            
            // set inLoop if we are in a loop but there is a sub loop that reset loop bool
            inLoop = prevLoop;
        }
    }

    void STTypeVisitor::visit(AST::ReadInteger *p)
    {
        p->outType = Scanner::Token::Type::Int;
    }

    void STTypeVisitor::visit(AST::ReadLine *p)
    {
        p->outType = Scanner::Token::Type::String;
    }

    void STTypeVisitor::visit(AST::Print *p)
    {
        p->outType = Scanner::Token::Type::Void;
    }

    void STTypeVisitor::visit(AST::Call *p)
    {
        SymbolTable::Scope *func( nullptr );
        try 
        {
            SymbolTable::IdEntry *entry = p->pScope->idLookup(p->value.getValue<std::string>());

            if (entry == nullptr)
            {
                std::stringstream ss;
                ss << "No declaration for function '" << p->value.getValue<std::string>() << "'";
                printTypeError(p->value, ss.str());
                return;
            }
            // set out type here since we have function entry
            p->outType = entry->type;
            // get scope to verify arguments
            func = p->pScope->funcLookup(entry);
        }
        catch (std::exception &ex)
        {
            return;
        }
        int i = 0;
        // std::cout << "Current Scope: " << p->pScope->toString(i);
        if (func != nullptr )
        {
            if (p->actuals.size() != func->numOfParams)
            {
                std::stringstream ss;
                ss << "Function '" << p->value.getValue<std::string>() << "' expects " << func->numOfParams << " arguments but " << p->actuals.size() << " given";
                printTypeError(p->value, ss.str());
            }

            int i = 0;
            // std::cout << "Function call scope: " <<  func->toString(i);
            // verify parameters
            for ( auto it = p->actuals.cbegin(); it != p->actuals.cend(); ++it)
            {
                exprErr = false;
                (*it)->accept(this);
                // it->visit(this);
                for( auto& formal : func->table )
                {
                    if (formal.second->block == i)
                    {
                        if ((*it)->outType != formal.second->type && ! exprErr)
                        {
                            std::stringstream ss;
                            std::string ltype ( Scanner::Token::getTypeName(formal.second->type));
                            std::string rtype ( Scanner::Token::getTypeName((*it)->outType));
                            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
                            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
                            
                            ss << "Incompatible argument " << i+1 << ": " << rtype << " given, " << ltype << " expected";
                            printTypeError((*it), p->value.lineNumber, p->value.lineInfo, ss.str());

                            // if ( dynamic_cast<AST::Value*>((*it)) != nullptr )
                            // {
                            //     printTypeError(dynamic_cast<AST::Value*>((*it))->value, ss.str());
                            // } else {
                            //     std::cout << "Invalid node\n";
                            // }
                            
                        }
                    }
                }

                i++;
            }
        }
    }

    void STTypeVisitor::visit(AST::Break *p)
    {
        if (! inLoop )
        {
            printTypeError(p->value, "break is only allowed inside a loop");
        }
    }

    void STTypeVisitor::visit(AST::Return *p)
    {
        // Get type of expression if there is one available
        if ( p->expr != nullptr )
        {
            // set the out type of the return 
            p->expr->accept(this);
            p->outType = p->expr->outType;
        }
        else
        {
            p->outType = Scanner::Token::Type::Void;
        }
        Scanner::Token::Type returnType( p->pScope->getReturnType() );

        if ( returnType != p->outType && ! exprErr)
        {
            std::string ltype( Scanner::Token::getTypeName( returnType ) );
            std::string rtype( Scanner::Token::getTypeName( p->outType ) );
            std::transform(ltype.begin(), ltype.end(), ltype.begin(), ::tolower);
            std::transform(rtype.begin(), rtype.end(), rtype.begin(), ::tolower);
                            
            std::stringstream ss;

            ss  << "Incompatible return: " 
                << rtype << " given, " << ltype << " expected";
            
            printTypeError(p->expr, p->value.lineNumber, p->value.lineInfo, ss.str());

            // if (dynamic_cast<AST::Value*>(p->expr) != nullptr )
            //     printTypeError(dynamic_cast<AST::Value*>(p->expr)->value, ss.str());
            // else if (dynamic_cast<AST::Expr*>(p->expr) != nullptr )
            //     printTypeError(dynamic_cast<AST::Expr*>(p->expr)->op, ss.str());
            // else
            //     printTypeError(p->value, ss.str());
        }
    }

    void STTypeVisitor::visit(AST::For *p)
    {
        inLoop = true;
        // verify loop expressions (start, cond, end) are type valid
        if (p->startExpr != nullptr)
            p->startExpr->accept(this);
        
        exprErr = false;
        p->expr->accept(this);

        if (p->expr->outType != Scanner::Token::Type::Bool && ! exprErr)
        {
            std::stringstream ss;
            ss << "Test expression must have boolean type";
            printTypeError(p->expr, p->value.lineNumber, p->value.lineInfo, ss.str());
        }

        exprErr = false;
        if (p->loopExpr != nullptr)
            p->loopExpr->accept(this);

        // verify statements are type valid
        p->stmt->accept(this);
        inLoop = false;
    }

    void STTypeVisitor::visit(AST::If *p)
    {
        // verify expression is type valid
        p->expr->accept(this);

        if (p->expr->outType != Scanner::Token::Type::Bool && ! exprErr)
        {
            std::stringstream ss;
            ss << "Test expression must have boolean type";
            printTypeError(dynamic_cast<AST::Expr*>(p->expr)->op, ss.str());
        }
        // verify statement or statement block is type valid
        p->stmt->accept(this);

        if (p->elseStmt != nullptr)
        {
            p->elseStmt->accept(this);
        }
    }

    void STTypeVisitor::visit(AST::While *p)
    {
        inLoop = true;
        // verify expression is type valid
        p->expr->accept(this);
        
        if (p->expr->outType != Scanner::Token::Type::Bool && ! exprErr)
        {
            std::stringstream ss;
            ss << "Test expression must have boolean type";
            printTypeError(dynamic_cast<AST::Expr*>(p->expr)->op, ss.str());
        }
        // verify statement or statement block is type valid
        p->stmt->accept(this);
        inLoop = false;
    }

    void STTypeVisitor::visit(AST::FunctionDeclaration *p)
    {
        // scope should hold parameters to function
        p->stmts->accept(this);
    }

    void STTypeVisitor::visit(AST::Program *p)
    {
        for ( auto &func : p->func)
        {
            func->accept(this);
        }
    }
};