
#include "CodeGenVisitor.hpp"

namespace CodeGen {

    void generate(AST::Program *p)
    {
        CodeGenVisitor v;
        p->accept(&v);

        return;
    }

    Immediate::Immediate()
    {

    }

    Immediate::Immediate(std::string value)
        : immediate(value)
    {

    }

    std::string Immediate::emit()
    {
        return immediate;
    }

    Label::Label()
    {

    }

    Label::Label(std::string value)
        : label(value)
    {

    }

    std::string Label::emit()
    {
        return label;
    }


    std::string Register::emit()
    {
        std::stringstream ss;
        
        ss << "$" << name;

        return ss.str();
    }

    std::string Memory::emit()
    {
        std::stringstream ss;

        ss << offset << "(" << baseReg.emit() << ")";

        return ss.str();
    }

    Comment::Comment()
    {

    }

    Comment::Comment(std::string comment)
        : comment(comment)
    {

    }

    std::string Comment::emit()
    {
        std::stringstream ss;

        ss << "; " << comment;

        return ss.str();
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

    std::string Instruction::emit()
    {
        std::stringstream ss;

        ss  << op << " "
            << operand1->emit() << " ";

        if ( operand2 != nullptr )
            ss << operand2->emit() << " ";
        
        if ( operand3 != nullptr )
            ss << operand3->emit() << " ";

        return ss.str();
    }

    CodeGenVisitor::CodeGenVisitor()
    {

    }







    void CodeGenVisitor::visit(AST::Program *p)
    {
        std::cout << "Beginning code gen of program\n";
    }
}