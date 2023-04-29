
#include "CodeGenVisitor.hpp"

namespace CodeGen {

    void generate(AST::Program *p)
    {
        CodeGenVisitor v;
        p->accept(&v);

        return;
    }

    Instruction::Instruction() 
    {

    }

    Instruction::Instruction(std::string op, Location* op1)
        : op(op)
        , operand1(op1)
    {

    }

    Instruction::Instruction(std::string op, Location* op1, Location* op2)
        : op(op)
        , operand1(op1)
        , operand2(op2)
    {

    }
    
    Instruction::Instruction(std::string op, Location* op1, Location* op2, Location* op3)
        : op(op)
        , operand1(op1)
        , operand2(op2)
        , operand3(op3)
    {

    }

    CodeGenVisitor::CodeGenVisitor()
    {

    }







    void CodeGenVisitor::visit(AST::Program *p)
    {
        std::cout << "Beginning code gen of program\n";
    }
}