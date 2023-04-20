#pragma once

#include <AST/AbstractSyntaxTree.hpp>

namespace SymbolTable {

    class IdEntry {

        public:
            IdEntry( std::string ident, Scanner::Token::Type type, int block, bool func = false )
                : ident(ident)
                , type(type)
                , func(true)
                , block(block)
                , offset(0)
            {
                std::cout   << "Adding entry for ident: " << ident << " " 
                            << Scanner::Token::getTypeName(type) << " at block " 
                            << block << std::endl;
            };

            std::string             ident;
            Scanner::Token::Type    type;
            bool                    func;

            /*
                1 == global
                2 == parameters
                3 == local
            */
            int block;

            /*
                offset from base pointer used for code gen
            */
            int offset;

    };

    class Scope {

        public:
            Scope()
                : parentScope(nullptr)
                , table()
                , funcScope()
            {};


            Scope *parentScope;

            typedef std::map<std::string, IdEntry>::iterator TableIterator;
            std::map<std::string, IdEntry> table;
            std::map<std::string, Scope*> funcScope;


            IdEntry install(std::string id, Scanner::Token::Type type, int block);
            IdEntry install(AST::Declaration*, int block);
            IdEntry install(AST::FunctionDeclaration*, int block);
            // IdEntry install(AST::StatementBlock*, int block);
            IdEntry idLookup(std::string);

            Scope * funcLookup(IdEntry entry);

            std::string toString(int &space);
    };

};