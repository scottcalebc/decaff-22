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

            // Special Type for parsing
            EMPTY,

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

        enum class SubType {
            Operand,
            Call,
            Assign,
            Paren,
            Comma,
            And,
            Or,
            LessThan,
            LessEqual,
            GreaterThan,
            GreaterEqual,
            Equal,
            NotEqual,
            Subtract,
            Add,
            Divide,
            Multiply,
            Modulus,
            Not,
            UnaryNegative,
        };

        static std::map<Type, std::string> enumName;

        static int identifierMaxLength;

        static std::string getTTypeName(const Type &type) {
            return "T_" + getTypeName(type);
        };

        static std::string getTypeName(const Type &type) {
            return (enumName.find(type) != enumName.end()) ? 
                enumName.find(type)->second :
                std::string("ERROR");
        };

        // usefule identifier keywords for quick lookup
        static std::map<std::string, Type> keywords;

        // useful multiple character operators for quick lookup
        static std::map<std::string, Type> operators;

        static std::map<std::string, SubType> subTypes;

        Type type;
        SubType subType;
        std::string value;
        std::string lineInfo;
        int lineNumber;
        int colStart;       // only column start since column end can be inferred by colStart + tokenString.len()

        Token():
            type(Type::END),
            subType(SubType::Operand),
            value(""),
            lineNumber(-1),
            colStart(-1)
        {};

        Token(Scanner::Token::Type type)
            : type(type)
            , subType(SubType::Operand)
            , value("")
            , lineNumber(-1)
            , colStart(-1)
        {};


        template<typename TokenValue>
        const TokenValue getValue() const;

        bool operator== (const Token& o)
        {
            if (
                type == o.type &&
                value.compare(o.value) == 0 &&
                lineNumber == o.lineNumber &&
                colStart == o.colStart
            )
                return true;

            return false;
        };
    };
}

std::ostream& operator<<(std::ostream &out, Scanner::Token const& token);



// namespace Token {
