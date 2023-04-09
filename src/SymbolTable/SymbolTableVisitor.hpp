

#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>

namespace SymbolTable
{
    class STVisitor: public Visitor {
            public:
                STVisitor() 
                {};

                void visit(Acceptor *a) { std::cout << "Got acceptor\n"; };
        };
}