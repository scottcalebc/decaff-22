#pragma once

#include <visitor/visitor.hpp>
#include "AbstractSyntaxTree.hpp"

namespace AST
{


        class ParseTreeConverter: public Converter {
            public:
                ParseTreeConverter() 
                        : pNode()
                {};

                Node* pNode;

                // these are abstract / inherited types 
                void convert(Parser::ParseNode *p) {};
                void convert(Parser::DeclarationType *p) {};
                void convert(Parser::BinaryExpression *p) {};
                void convert(Parser::Expression *p) {};
                void convert(Parser::Statement *p) {};
                void convert(Parser::Actual *p) {};
                void convert(Parser::KeywordStmt *p) {};
                void convert(Parser::ReturnType *p) {};
                void convert(Parser::Declarations *p) {};

                // todo ccs need to implement
                void convert(Parser::ParenExpr *p){};
                void convert(Parser::UnaryExpression *p){};
                void convert(Parser::AssignExpression *p){};
                void convert(Parser::ArithmeticExpression *p){};
                void convert(Parser::LogicalExpression *p){};
                void convert(Parser::RelationalExpression *p){};
                void convert(Parser::EqualityExpression *p){};
                void convert(Parser::VariableDeclaration *p){};
                void convert(Parser::FormalVariableDeclaration *p){};
                void convert(Parser::CallExpression *p){};
                void convert(Parser::PrintStmt  *p){};
                void convert(Parser::ReadIntExpr *p){};
                void convert(Parser::ReadLineExpr *p){};
                void convert(Parser::BreakStmt  *p){};
                void convert(Parser::WhileStmt  *p){};
                void convert(Parser::IfStmt *p){};
                void convert(Parser::ForStmt *p){};
                
                
                void convert(Parser::LValue  *p);
                void convert(Parser::Constant *p);
                void convert(Parser::Identifier *p);
                void convert(Parser::ReturnStmt *p);
                void convert(Parser::StatementBlock *p);
                void convert(Parser::FunctionDeclaration *p);

                void convert(Parser::Program *p);
        };


} // namespace AST
