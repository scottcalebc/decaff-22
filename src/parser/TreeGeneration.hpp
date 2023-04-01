#pragma once 

#include "lexer/lexer.hpp"

namespace Parser {
    /**
     * @brief Starts Parser Tree generation by taking in a lexer object for token stream
     * 
     * @param lexer 
     */
    void treeGeneration(Scanner::Lexer *lexer);
};