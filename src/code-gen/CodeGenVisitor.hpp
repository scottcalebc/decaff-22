#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>

#include "Entities.hpp"


namespace CodeGen {
    void generate(AST::Program *p, std::string file_name);
    
    class CodeGenVisitor: public Visitor {

        public:
            CodeGenVisitor();

            int tmpCounter;
            int labelCounter;

            Label *endLoop;  // Used by Break

            void emit(Label* label);
            void emit(Comment *output);
            void emit(std::string output);
            void emit(std::string output, int *dataSize);
            void emit(std::string op, Location* operand1);
            void emit(std::string op, Location* operand1, 
                Location* operand2);
            void emit(std::string op, Location* operand1, 
                Location* operand2, Location* operand3);

            // used to add comment to last emitted instruction
            void addComment(Comment* comment);


            std::vector<InstructionStreamItems*> instructions;

            void write(std::string fileName);

            // Keep references to an instance Register & Memory
            // this will allow for calls to Next() for next available register and

            Memory * setupSubExpr(AST::Node *left, 
                AST::Node *right, SymbolTable::Scope *pScope, 
                std::string &tmpName);

            void loadSubExpr(AST::Node *p, Register *reg);
            void loadSubExprs(AST::Node *left, Register *lreg, 
                AST::Node *right, Register *rreg);

            void saveSubExpr(Register *reg, Memory* mem, 
                std::string tmpName);

            void identCheck(AST::Node *p, SymbolTable::Scope *pScope);

            void binaryExpr(AST::Expr *p, std::string op);
            void unaryExpr(AST::Expr *p, std::string op);

            void pushParam(AST::Node *p);
            void popParams(int num);
            void CallFormalVisit(AST::Call *p);
            void CallFormalPush(AST::Call *p);
            void saveReturn(AST::Call *p, std::string tmpName);
            void functionReturn();

            // Overloads of Visitor Interface
            // default acceptor to show missing virtual functions
            void visit(Acceptor *a) { std::cout << "STTypeVisitor: Got acceptor" << std::endl; };

            // Ident will lookup in ST and return type
            void visit(AST::Ident *p);
            // Constant will simply set their type
            void visit(AST::Constant *p);

            // Expressions

            // arithmetic expressions
            void visit(AST::Add *p);
            void visit(AST::Assign *p);
            void visit(AST::Divide *p);
            void visit(AST::Modulus *p);
            void visit(AST::Subtract *p);
            void visit(AST::Multiply *p);

            // logical  epxressions
            void visit(AST::Or *p);
            void visit(AST::And *p);
            void visit(AST::Not *p);
            void visit(AST::GTE *p);
            void visit(AST::LTE *p);
            void visit(AST::Equal *p);
            void visit(AST::NotEqual *p);
            void visit(AST::LessThan *p);
            void visit(AST::GreaterThan *p);

            // function calls
            void visit(AST::Call *p);
            void visit(AST::Print *p);
            void visit(AST::ReadLine *p);
            void visit(AST::ReadInteger *p);
            
            // keyword statments, break has no outtype
            void visit(AST::If *p);
            void visit(AST::For *p);
            void visit(AST::Break *p);
            void visit(AST::While *p);
            void visit(AST::Return *p);
            void visit(AST::KeywordStmt *p) {};


            void visit(AST::StatementBlock *p);

            void visit(AST::Declaration *p);
            void visit(AST::FunctionDeclaration *p);
            void visit(AST::Program *p);

    };

    
}