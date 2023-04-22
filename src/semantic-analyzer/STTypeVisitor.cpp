#include <iostream>


#include "STTypeVisitor.hpp"


namespace SemanticAnalyzer {

    void typeCheck(AST::Program *p)
    {
        STTypeVisitor visitor;
        p->accept(&visitor);
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

            if (right != nullptr && ! unary)
            {
                right->accept(this);
                rtype = right->outType;


                if (ltype == rtype)
                {
                    std::cout << "Type match for ltype (" 
                        << Scanner::Token::getTypeName(ltype)
                        << ") to rtype of: " << Scanner::Token::getTypeName(rtype) << std::endl;
                    return true;
                }
                else
                {
                    std::cout << "Type mismatch for ltype (" 
                        << Scanner::Token::getTypeName(ltype)
                        << ") to rtype of: " << Scanner::Token::getTypeName(rtype) << std::endl;
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

    void STTypeVisitor::visit(AST::Modulus *p)
    {
        std::cout << "Evaluating modulus\n";
        if (binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType))
        {
            p->outType = p->left->outType;
        }else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }
    }

    void STTypeVisitor::visit(AST::Divide *p)
    {
        std::cout << "Evaluating divide\n";
        if (binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType))
        {
            p->outType = p->left->outType;
        }else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }
    }

    void STTypeVisitor::visit(AST::Multiply *p)
    {
        std::cout << "Evaluating Multiply\n";
        if (binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType))
        {
            p->outType = p->left->outType;
        }else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }
    }

    void STTypeVisitor::visit(AST::Subtract *p)
    {
        std::cout << "Evaluating Subtract\n";
        bool unary = p->right == nullptr;
        if (binaryTypeCheck(p->left, p->right, unary) && arithmeticCheck(p->left->outType))
        {
            p->outType = p->left->outType;
        }else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }
    }

    void STTypeVisitor::visit(AST::Add *p)
    {
        std::cout << "Evaluating Add\n";
        if (binaryTypeCheck(p->left, p->right) && arithmeticCheck(p->left->outType) )
        {
            p->outType = p->left->outType;
        }else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }
    }

    void STTypeVisitor::visit(AST::Assign *p)
    {
        std::cout << "Evaluating Assign\n";

        if (binaryTypeCheck(p->left, p->right))
        {
            p->outType = p->left->outType;
        } else {
            std::cout << "Error on line: " << p->op.lineInfo << std::endl;
        }

    }

    void STTypeVisitor::visit(AST::StatementBlock *p)
    {
        for(auto &stmt : p->stmts)
        {
            stmt->accept(this);
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
        std::cout << "Got call for ident " << p->value;
        SymbolTable::Scope *func( nullptr );
        try 
        {
            SymbolTable::IdEntry *entry = p->pScope->idLookup(p->value.getValue<std::string>());
            // set out type here since we have function entry
            std::cout << "Setting type for call to: " << Scanner::Token::getTypeName(entry->type) << std::endl;
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
                std::cout << "Invalid num of params, expected: " << func->numOfParams << std::endl;
            }

            int i = 0;
            // std::cout << "Function call scope: " <<  func->toString(i);
            // verify parameters
            for ( auto it = p->actuals.crbegin(); it != p->actuals.crend(); ++it)
            {
                (*it)->accept(this);
                // it->visit(this);
                for( auto& formal : func->table )
                {
                    if (formal.second->block == i)
                    {
                        if ((*it)->outType != formal.second->type)
                        {
                            std::cout << "Error: expected " << Scanner::Token::getTypeName(formal.second->type) << " for expr\n";
                        }
                    }
                }

                i++;
            }
        }
    }

    void STTypeVisitor::visit(AST::FunctionDeclaration *p)
    {
        // scope should hold parameters to function
        p->stmts->accept(this);
    }

    void STTypeVisitor::visit(AST::Program *p)
    {
        std::cout << "STTypeVisitor: Entering program\n";

        for ( auto &func : p->func)
        {
            func->accept(this);
        }
    }
};