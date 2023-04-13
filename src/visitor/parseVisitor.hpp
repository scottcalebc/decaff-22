#pragma once


#include "common/VisitorForward.hpp"
#include <common/ParserForward.hpp>

#include <parser/ParseTree.hpp>


/**
 * @brief Interface to convert ParseTree to AST
 * 
 */
class Converter {
    protected:
        Converter() {};
    
    public:
        virtual void convert(Parser::ParseNode *p) = 0;
        virtual void convert(Parser::Identifier *p) = 0;
        virtual void convert(Parser::DeclarationType *p) = 0;
        virtual void convert(Parser::ReturnType *p) = 0;
        virtual void convert(Parser::Statement *p) = 0;
        virtual void convert(Parser::Declarations *p) = 0;
        virtual void convert(Parser::Expression *p) = 0;
        virtual void convert(Parser::BinaryExpression *p) = 0;
        virtual void convert(Parser::UnaryExpression *p) = 0;
        virtual void convert(Parser::AssignExpression *p) = 0;
        virtual void convert(Parser::ArithmeticExpression *p) = 0;
        virtual void convert(Parser::LogicalExpression *p) = 0;
        virtual void convert(Parser::RelationalExpression *p) = 0;
        virtual void convert(Parser::EqualityExpression *p) = 0;
        virtual void convert(Parser::ParenExpr *p) = 0;
        virtual void convert(Parser::LValue  *p) = 0;
        virtual void convert(Parser::Constant *p) = 0;
        virtual void convert(Parser::VariableDeclaration *p) = 0;
        virtual void convert(Parser::StatementBlock  *p) = 0;
        virtual void convert(Parser::FormalVariableDeclaration *p) = 0;
        virtual void convert(Parser::FunctionDeclaration *p) = 0;
        virtual void convert(Parser::Actual *p) = 0;
        virtual void convert(Parser::CallExpression *p) = 0;
        virtual void convert(Parser::PrintStmt  *p) = 0;
        virtual void convert(Parser::ReadIntExpr *p) = 0;
        virtual void convert(Parser::ReadLineExpr *p) = 0;
        virtual void convert(Parser::KeywordStmt *p) = 0;
        virtual void convert(Parser::ReturnStmt *p) = 0;
        virtual void convert(Parser::BreakStmt  *p) = 0;
        virtual void convert(Parser::WhileStmt  *p) = 0;
        virtual void convert(Parser::IfStmt *p) = 0;
        virtual void convert(Parser::ForStmt *p) = 0;
        virtual void convert(Parser::Program *p) = 0;
};