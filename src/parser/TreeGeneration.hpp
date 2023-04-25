#pragma once 

#include "ParseTree.hpp"

#include "lexer/lexer.hpp"

namespace Parser {
    /**
     * @brief Starts Parser Tree generation by taking in a lexer object for token stream
     * 
     * @param lexer 
     */
    Program* treeGeneration(Scanner::Lexer *lexer, bool print=false);
};