#pragma once

#include <AST/AbstractSyntaxTree.hpp>

namespace SymbolTable {

    class IdEntry {

        public:
            IdEntry( std::string ident )
                : ident(ident)
                , type()
                , func(false)
                , block(0)
                , offset(0)
                {};
            IdEntry( std::string ident, Scanner::Token::Type type, int block, bool func = false )
                : ident(ident)
                , type(type)
                , func(func)
                , block(block)
                , offset(0)
            {
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
                , numOfParams(0)
                , returnType(Scanner::Token::Type::ERROR)
            {};


            Scope *parentScope;
            int numOfParams;                    // used by functions for type checking
            Scanner::Token::Type returnType;    // used by functions for type check

            // this will hold the offset necessary for the function scope
            // children scopes allocating locals/temporaries
            int baseOffset;                     // used during code gen


            typedef std::map<std::string, IdEntry*>::iterator TableIterator;
            std::map<std::string, IdEntry*> table;
            std::map<std::string, Scope*> funcScope;


            IdEntry* install(std::string id, Scanner::Token::Type type, int block);
            IdEntry* install(AST::Declaration*, int block);
            IdEntry* install(AST::FunctionDeclaration*, int block);

            IdEntry* fakeInstall(std::string id);
            // IdEntry install(AST::StatementBlock*, int block);
            IdEntry* idLookup(std::string);

            Scope * funcLookup(IdEntry *entry);
            Scanner::Token::Type getReturnType();

            int getNextOffset();

            std::string toString(int &space);
    };

};