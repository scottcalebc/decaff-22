#pragma once


#include <AST/AbstractSyntaxTree.hpp>



namespace SymbolTable {

    class IdEntry {

        public:
            std::string ident;
            Scanner::Token::Type type;

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

    void generate(AST::Program *prog);
}