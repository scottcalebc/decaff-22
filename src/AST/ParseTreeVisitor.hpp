#pragma once

#include <visitor/visitor.hpp>

#include "AbstractSyntaxTree.hpp"

namespace AST
{


        class ParseTreeConverter {
            public:
                template< typename Object >
                Node* visit(Object a) { return nullptr; };
        };


} // namespace AST
