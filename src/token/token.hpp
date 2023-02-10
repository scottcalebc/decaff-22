#pragma once
#include <ostream>
#include <string>
#include <map>

namespace Scanner {
    class Token {


        public:
        enum class Type {
            // Special ERROR to allow printing of errors without breaking main loop
            ERROR,

            // Symbols
            Operator,
            Separator,

            //Constant Values
            IntConstant,
            NullConstant,
            BoolConstant,
            DoubleConstant,
            StringConstant,

            // Anything not a symbol or constant is an identifier
            Identifier,
            
            // Primitive type names
            Int,
            Void,
            Bool,
            Double,
            String,

            // Control Statements
            If,
            For,
            Else,
            Break,
            While,
            Return,

            // Conditionals
            Or,
            And,
            Equal,
            NotEqual,
            LessEqual,
            GreaterEqual,

            // Special Type to represent finished traversing through all tokens
            END
        };

        static std::map<Type, std::string> enumName;

        static int identifierMaxLength;

        static std::string getTypeName(const Type &type) {
            return (enumName.find(type) != enumName.end()) ? 
                "T_" + enumName.find(type)->second :
                std::string("ERROR");
        };

        // usefule identifier keywords for quick lookup
        static std::map<std::string, Type> keywords;

        // useful multiple character operators for quick lookup
        static std::map<std::string, Type> operators;

        Type type;
        std::string value;
        int lineNumber;
        int colStart;       // only column start since column end can be inferred by colStart + tokenString.len()

        Token():
            type(Type::END),
            value(""),
            lineNumber(-1),
            colStart(-1)
        {};


        const std::string getValue() const;
    };
}

std::ostream& operator<<(std::ostream &out, Scanner::Token const& token);



// namespace Token {
