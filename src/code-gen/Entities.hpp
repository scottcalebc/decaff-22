#pragma once

#include <string>
    
namespace CodeGen {
    /**
     * @brief Abstract Base class for describing storage of location of variables / temporaries
     * 
     */
    class Location {

        protected:
            Location() {};

        public:
        // virtual methods to be overriden
            virtual std::string emit() = 0;
    };


    class Immediate : public Location {
        public:
            Immediate();
            Immediate(std::string value);
            std::string immediate;
            std::string emit();
    };

    /**
     * @brief Class for generating register locations
     * 
     */
    class Register : public Location {

        public:
            Register();
            Register(std::string reg);
            std::string name;   //holds the name of the register (t1-tN) (fp, sp, gb, ...)
            std::string emit();

    };

    /**
     * @brief Class for describing memory location
     * 
     */
    class Memory : public Location {

        public:
            Memory();
            Memory(std::string reg, int offset);
            Register    baseReg; // base register (frame pointer, stack pointer, global pointer)
            int         offset; // offset from register
            std::string emit();
    };

    class Comment;
    class InstructionStreamItems {
        protected:
            InstructionStreamItems() 
                : comment(nullptr)
            {};

        public:
            virtual std::string emit() = 0;
            Comment *comment;   //optionally comment to follow instruction
    };

    /**
     * @brief Allow for easy comment pushing with string
     *      This will allow certain backends to change comment preceding char
     *      (e.g. # or ;)
     * 
     */
    class Comment : public InstructionStreamItems {

        public:
            Comment();
            Comment(std::string comment);

            std::string comment;

            std::string emit();
    };

    /**
     * @brief Class to handle various commands that don't match typical instruction
     *      for, (e.g. .text, .data, etc.)
     * 
     */
    class Command : public InstructionStreamItems {
        public:
            Command();
            Command(std::string command);

            std::string command;

            std::string emit();
    };


    class Instruction : public InstructionStreamItems {

        public:
            Instruction();
            Instruction(std::string op, Location* op1);
            Instruction(std::string op, Location*op1, Location* op2);
            Instruction(std::string op, Location*op1, Location* op2, Location* op3);

            std::string op;     // string of operation 

            // Always Assume first operand is a register
            // caller will handle if op2/3 need to be registers, just assume their right
            Location *operand1; // some ops only have a single operand
            Location *operand2; // some ops only have two operands
            Location *operand3; // some ops have three

            


            std::string emit();
    };

    class Label :  public Location, public InstructionStreamItems {
        public:
            Label();
            Label(std::string value);
            std::string label;
            std::string emit();
    };

};