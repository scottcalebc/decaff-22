
#include "CodeGenVisitor.hpp"
#include "Entities.hpp"

#include <iomanip>

namespace CodeGen {

    void generate(AST::Program *p, std::string file_name)
    {
        CodeGenVisitor v;
        p->accept(&v);

        // possible optimization step

        v.write(file_name);

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
        std::stringstream ss;

        ss << label << ": ";

        if (comment != nullptr)
            ss << comment->emit() << " ";

        return ss.str();
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

        ss << "# " << comment;

        return ss.str();
    }

    Command::Command()
    {

    }

    Command::Command(std::string command)
        : command(command)
    {

    }

    std::string Command::emit()
    {
        std::stringstream ss;

        ss << "  " << command << " ";

        if (comment != nullptr)
            ss << comment << " ";

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

        ss  << "  " << op << " "
            << operand1->emit() << " ";

        if ( operand2 != nullptr )
            ss << operand2->emit() << " ";
        
        if ( operand3 != nullptr )
            ss << operand3->emit() << " ";

        if ( comment != nullptr )
            ss << comment->emit() << " ";

        return ss.str();
    }

    CodeGenVisitor::CodeGenVisitor()
        : instructions()
    {

    }

    /**
     * @brief These emit methods are different than the ones for the entities in the instruction
     *      this will simply create an instruction and place it in the stream to be emitted at a single
     *      point after all generation/optimization is finished
     * 
     */
    void CodeGenVisitor::emit(std::string output)
    {
        if (output.at(0) == '.' || output.at(0) == '_')
            instructions.push_back(new Command(output));
        else
            instructions.push_back(new Comment(output));
    }

    void CodeGenVisitor::emit(Label *label)
    {
        instructions.push_back(label);
    }

    void CodeGenVisitor::emit(std::string op, Location* operand1)
    {
        instructions.push_back(new Instruction(op, operand1));
    }

    void CodeGenVisitor::emit(std::string op, Location* operand1, Location* operand2)
    {
        instructions.push_back(new Instruction(op, operand1, operand2));
    }

    void CodeGenVisitor::emit(std::string op, Location* operand1, Location* operand2, Location* operand3)
    {
        instructions.push_back(new Instruction(op, operand1, operand2, operand3));
    }

    void CodeGenVisitor::write(std::string file_name)
    {
        std::stringstream ss;
        ss << file_name << ".s";


        std::string actual_file_name( ss.str() );

        std::cout << "Opening file with name: " << actual_file_name << std::endl;
        std::ofstream file;
        file.open( actual_file_name );

        for( auto instrs : instructions )
        {
            
            std::string spacing("\t");
            if (dynamic_cast<Label*>(instrs) != nullptr )
                spacing = "  ";

            std::string instr(instrs->emit());
            file    << spacing
                    << instr << std::endl;

        }

        file.close();
    }





    void CodeGenVisitor::visit(AST::FunctionDeclaration *p)
    {
        std::string funcName = p->ident.getValue<std::string>();
        std::cout << "Staring gen of function: " << p->ident.getValue<std::string>() << std::endl;

        if (funcName.compare("main") == 0)
        {
            emit(new Label(funcName));
            instructions.back()->comment = new Comment("Start of main function");
        }
    }

    void CodeGenVisitor::visit(AST::Program *p)
    {
        std::cout << "Beginning code gen of program with preamble\n";
        emit("standard Decaf preamble");
        emit(".text");
        emit(".align 2");
        emit(".globl main");

        std::cout << "Visiting declarations to assign memory locations\n";

        std::cout << "Visiting functions for generation\n";
        for( auto func : p->func )
        {
            func->accept(this);
        }
    }
}