#include <iostream>
#include <unistd.h>
#include <algorithm>

// lexer includes to run only lexer
#include "lexer/lexer.hpp"
#include "lexer/exceptions.hpp"

// parser includes to run parsing after lexing input
#include "parser/TreeGeneration.hpp"
#include "parser/exceptions.hpp"

#include <AST/AbstractSyntaxTree.hpp>

#include <SymbolTable/generate.hpp>
#include <semantic-analyzer/STTypeVisitor.hpp>

#include <code-gen/CodeGenVisitor.hpp>

#define VERSION 0.1.0

std::vector<std::string> vecFunctions{
    "--parser",
    "--lexer",
    "--semantic-check",
    "--code-gen"
};

int usage(const char* progName)
{
    std::cerr << "Usage: " << progName << " <file_path>" << std::endl;
    std::cerr << "  file_path    -   path to source file to convert" << std::endl;

    return 1;
}

std::string getFileName(const std::string &file_path)
{
    std::size_t start = file_path.find_last_of("/");
    std::size_t end = file_path.find_last_of(".");

    return file_path.substr(start+1, end-start-1);
}

int main(int argc, char** argv) {

    if (argc < 2 || argc > 3)
    {
        usage(argv[0]);
    }

    std::string function("--parser");
    std::string file_path(argv[1]);

    if (argc > 2 )
    {
        function = std::string(argv[1]);
        file_path = std::string(argv[2]);

        auto it = std::find(vecFunctions.begin(), vecFunctions.end(), function);

        if (it == vecFunctions.end())
        {
            std::cerr << "Invalid function: " << function << std::endl << std::endl;
            return usage(argv[0]);
        }
    }
    
    std::string file_name( getFileName(file_path) );
    Scanner::Lexer lexer(file_path);

    if (function.compare("--lexer") == 0)
    {
        // convert file to tokens
        Scanner::Token token;
        do {
            try {
                // std::cout << "Getting Token..."<< std::endl;
                token = lexer.getNextToken();

                std::cout << token;
            } 
            catch( Scanner::GenericException &exc ) {
                std::cout << exc.what() << std::endl;
                token.type = Scanner::Token::Type::ERROR;
            }
        } while (token.type != Scanner::Token::Type::END);
        
        // std::cout << "Ended at lexer function" << std::endl;
        return 0;
    }
    // we continue onto parser
    // convert file to AST
    AST::Program prog;

    try {
        prog = AST::Program( Parser::treeGeneration( &lexer, function.compare("--parser") == 0 ) );

        // std::cout << "Generating symbol tree\n";
        SymbolTable::generate(&prog);

    }
    catch ( Parser::ParseException &exc)
    {
        std::cout << exc.what() << std::endl;
    }
    
    if (function.compare("--parser") == 0)
    {
        // std::cout << "Ended at parser function" << std::endl;
        return 0;
    }
    bool bTypeCheck(true);
    try {
        bTypeCheck = SemanticAnalyzer::typeCheck(&prog);
    }
    // Some Semantic checking can be "recoverable or at least ignore later invocations of issues"
    // 
    catch ( std::exception &exc )
    {
        std::cout << exc.what() << std::endl;
    }

    // stop after semantic checking
    if (function.compare("--semantic-check") == 0)
    {
        // std::cout << "Ended at semantic-check function" << std::endl;
        return 0;
    }

    //linking stage
    if (bTypeCheck && prog.pScope->idLookup("main") == nullptr)
    {
        bTypeCheck = false;
        std::cout   << "\n*** Error.\n"
                    << "*** Linker: function 'main' not defined\n\n";

    }

    // code gen
    if (bTypeCheck)
        CodeGen::generate(&prog, file_name);

    return 0;
}
