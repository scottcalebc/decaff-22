#pragma once
#include <ostream>
#include <string>
#include <map>

class Token {


    public:
    enum class Type {
        // Symbols
        Operator,
        Separator,
        
        //Constant Values
        IntConstant,
        BoolConstant,
        DoubleConstant,
        StringConstant,

        // Anything not a symbol or constant is an identifier
        Identifier,
        
        // Primitive type names
        Int,
        Void,
        Double,
        String,

        // Control Statements
        If,
        Else,
        Break,
        While,

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

    static std::string getTypeName(const Type &type) {
        return (enumName.find(type) != enumName.end()) ? 
            "T_" + enumName.find(type)->second :
            std::string("ERROR");
    };

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

    friend std::ostream& operator<<(std::ostream &out, Token const& token);
};




// namespace Token {
