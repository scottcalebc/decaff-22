#pragma once

#include <iostream>

#include <visitor/astVisitor.hpp>
#include <AST/AbstractSyntaxTree.hpp>


namespace CodeGen {
    void generate(AST::Program *p);

    /**
     * @brief Abstract Base class for describing storage of location of variables / temporaries
     * 
     */
    class Location {

        protected:
            Location() {};

        public:
        // virtual methods to be overriden
    };


    class Immediate {
        public:
            std::string immediate;
    };

    /**
     * @brief Class for generating register locations
     * 
     */
    class Register : public Location {

        public:
            std::string name;   //holds the name of the register (t1-tN) (fp, sp, gb, ...)

    };

    /**
     * @brief Class for describing memory location
     * 
     */
    class Memory : public Location {

        public:
            Register    baseReg; // base register (frame pointer, stack pointer, global pointer)
            int         offset; // offset from register
    };


    class Instruction {

        public:
            Instruction();
            Instruction(std::string op, Location* op1);
            Instruction(std::string op, Location*op1, Location* op2);
            Instruction(std::string op, Location*op1, Location* op2, Location* op3);

            std::string op;     // string of operations

            Location *operand1; // some ops only have a single operand
            Location *operand2; // some ops only have two operands
            Location *operand3; // some ops have three
    };

    
    class CodeGenVisitor: public Visitor {

        public:
            CodeGenVisitor();

            // Overloads of Visitor Interface
            // default acceptor to show missing virtual functions
            void visit(Acceptor *a) { std::cout << "STTypeVisitor: Got acceptor" << std::endl; };

            // Ident will lookup in ST and return type
            void visit(AST::Ident *p){};
            // Constant will simply set their type
            void visit(AST::Constant *p){};

            // Expressions

            // arithmetic expressions
            void visit(AST::Add *p){};
            void visit(AST::Assign *p){};
            void visit(AST::Divide *p){};
            void visit(AST::Modulus *p){};
            void visit(AST::Subtract *p){};
            void visit(AST::Multiply *p){};

            // logical  epxressions
            void visit(AST::Or *p){};
            void visit(AST::And *p){};
            void visit(AST::Not *p){};
            void visit(AST::GTE *p){};
            void visit(AST::LTE *p){};
            void visit(AST::Equal *p){};
            void visit(AST::NotEqual *p){};
            void visit(AST::LessThan *p){};
            void visit(AST::GreaterThan *p){};

            // function calls
            void visit(AST::Call *p){};
            void visit(AST::Print *p){};
            void visit(AST::ReadLine *p){};
            void visit(AST::ReadInteger *p){};
            
            // keyword statments, break has no outtype
            void visit(AST::If *p){};
            void visit(AST::For *p){};
            void visit(AST::Break *p){};
            void visit(AST::While *p){};
            void visit(AST::Return *p){};
            void visit(AST::KeywordStmt *p) {};


            void visit(AST::StatementBlock *p){};
            void visit(AST::FunctionDeclaration *p){};
            void visit(AST::Program *p);

    };

    
}