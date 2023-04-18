#include <iostream>


#include "STTypeVisitor.hpp"


namespace SemanticAnalyzer {

    void typeCheck(AST::Program *p)
    {
        STTypeVisitor visitor;
        p->accept(&visitor);
    }

    void STTypeVisitor::visit(AST::Ident *p)
    {
        int space(0);
        // need to check if symbol is in table
        std::cout << "Checking for symbol in table: " 
            << p->pScope->toString(space);
        SymbolTable::IdEntry entry = p->pScope->idLookup(p->value.getValue<std::string>());

        // need to handle if id doesn't exist in table;
        p->outType = entry.type;
    }

    void STTypeVisitor::visit(AST::Constant *p)
    {
        p->outType = p->value.type;
    }

    void STTypeVisitor::visit(AST::Add *p)
    {
        std::cout << "Evaluating Add\n";
        p->left->accept(this);
        p->right->accept(this);
    }

    void STTypeVisitor::visit(AST::Assign *p)
    {
        std::cout << "Evaluating Assign\n";
        Scanner::Token::Type ltype;
        Scanner::Token::Type rtype;

        if (p->left != nullptr)
        {
            p->left->accept(this);
            ltype = p->left->outType;

            if (p->right != nullptr)
            {
                p->right->accept(this);
                rtype = p->right->outType;


                if (ltype == rtype)
                    p->outType = ltype;
                else
                {
                    std::cout << "Type mismatch cannot assign ltype (" 
                        << Scanner::Token::getTypeName(ltype)
                        << ") to rtype of: " << Scanner::Token::getTypeName(rtype) << std::endl;
                }
            }
            else
            {
                std::cout << "Cannot get right type from empty expression\n";
            }
        }
        else
        {
            std::cout << "Cannot get left type from empty expression\n";
        }


    }

    void STTypeVisitor::visit(AST::StatementBlock *p)
    {
        for(auto &stmt : p->stmts)
        {
            stmt->accept(this);
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