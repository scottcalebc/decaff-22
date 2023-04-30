
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
        : immediate()
        , value(nullptr)
    {

    }

    Immediate::Immediate(std::string value)
        : immediate(value)
        , value(nullptr)
    {

    }

    Immediate::Immediate(int* value)
        : immediate()
        , value(value)
    {

    }

    std::string Immediate::emit()
    {
        std::stringstream ss;

        if (value == nullptr)
            ss << immediate;
        else
            ss << *value - 4;
        
        return ss.str();
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

    Register::Register()
    {

    }

    Register::Register(std::string reg)
        : name(reg)
    {

    }

    // Using three registers
    int Register::registerCount = 3;
    int Register::registerIndex = 0;


    std::string Register::emit()
    {
        std::stringstream ss;
        
        ss << "$" << name;

        return ss.str();
    }

    Register * Register::Next()
    {
        registerIndex = (registerIndex + 1)%registerCount;
        std::stringstream ss;
        ss << "t" << registerIndex;
        return new Register(ss.str());
    }

    void Register::Free()
    {
        registerIndex = (registerIndex - 1)%registerCount;
    }

    Memory::Memory()
    {

    }

    Memory::Memory(std::string reg, int offset)
        : baseReg(Register(reg))
        , offset(offset)
    {

    }

    std::string Memory::emit()
    {
        std::stringstream ss;

        ss << offset << "(" << baseReg.emit() << ")";

        return ss.str();
    }

    Comment::Comment()
        : comment()
        , dataSize(nullptr)
    {
    }

    Comment::Comment(std::string comment)
        : comment(comment)
        , dataSize(nullptr)
    {

    }

    Comment::Comment(std::string comment, int *dataSize)
        :comment(comment)
        , dataSize(dataSize)
    {

    }

    std::string Comment::emit()
    {
        std::stringstream ss;

        ss << "# " << comment;

        if (dataSize != nullptr)
            ss << " " << *dataSize - 4;

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
            << operand1->emit();

        if ( operand2 != nullptr )
            ss << ", " << operand2->emit();
        
        if ( operand3 != nullptr )
            ss << ", " << operand3->emit() << " ";

        if ( comment != nullptr )
            ss << "\t" << comment->emit() << " ";

        return ss.str();
    }

    CodeGenVisitor::CodeGenVisitor()
        : instructions()
        , tmpCounter(0)
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
    
    void CodeGenVisitor::emit(Comment *c)
    {
        instructions.push_back(c);
    }

    void CodeGenVisitor::emit(std::string output, int *dataSize)
    {
        instructions.push_back(new Comment(output, dataSize));
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

    void CodeGenVisitor::addComment(Comment* comment)
    {
        instructions.back()->comment = comment;
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










    void CodeGenVisitor::visit(AST::Ident *p)
    {
        // lookup in symbol table to retrieve register
        SymbolTable::IdEntry *e = p->pScope->idLookup(p->value.getValue<std::string>());

        p->reg = e->reg;

        std::string reg("fp");

        // TODO change how block is used for parameters
        if (e->block == 0)
            reg = "gp";

        p->mem = new Memory(reg, e->offset);
        p->memName = p->value.getValue<std::string>();
    }

    void CodeGenVisitor::visit(AST::Constant *p)
    {
        // load constant into temp
        // get next offset for temporary

        int offset = p->pScope->getNextOffset();
        Memory *mem = new Memory("fp", -offset);

        std::cout << "Visiting constant calculating tmp label\n";
        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;
        std::string tmp(ss.str());
        std::string constant(p->value.getValue<std::string>());

        emit(new Comment(tmp + " = " + constant));

        Register *reg = Register::Next(); //get next register

        switch (p->value.type)
        {
            case Scanner::Token::Type::StringConstant:
            case Scanner::Token::Type::BoolConstant:
                break;
            default:
                emit("li", reg, new Immediate(constant));
                addComment(new Comment("load constant value " + constant + " into " + reg->emit() ));
                emit("sw", reg, mem);
                addComment(new Comment("spill " + tmp + " from " + reg->emit() + " to " + mem->emit()));
                Register::Free();

        }

        p->mem = mem;
        p->memName = tmp;
    }

    void CodeGenVisitor::visit(AST::Add *p)
    {
        std::cout << "Starting addition\n";

        p->left->accept(this);
        p->right->accept(this);

        int offset = p->pScope->getNextOffset();
        Memory *mem = new Memory("fp", -offset);

        std::cout << "Visiting addition calculating tmp label\n";
        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;
        std::string tmp(ss.str());

        int start( p->minCol() );
        int end( p->maxCol() );

        if (p->left->mem != nullptr && p->right->mem != nullptr)
        {
            // load right location
            Register *rreg = Register::Next();
            Register *lreg = Register::Next();
            Register *oreg = Register::Next();

            emit(new Comment( tmp + " = " + p->op.lineInfo.substr(start-1, end-start)) ); // expression start
            emit("lw", rreg, p->right->mem);
            addComment(new Comment("fill " + p->right->memName + " to " + rreg->emit() + " from " + p->right->mem->emit()));
            emit("lw", lreg, p->left->mem);
            addComment(new Comment("fill " + p->left->memName + " to " + lreg->emit() + " from " + p->left->mem->emit()));

            // add instr
            emit("add", oreg, lreg, rreg);
            
            // save to mem
            emit("sw", oreg, mem);
            addComment(new Comment("spill " + tmp + " from " + oreg->emit() + " to " + mem->emit()));


            if (dynamic_cast<AST::Ident*>(p->left) != nullptr)
            {
                AST::Ident* ident = dynamic_cast<AST::Ident*>(p->left);
                SymbolTable::IdEntry *e = p->pScope->idLookup(ident->value.getValue<std::string>());

                if (e != nullptr && ! e->loaded)
                    std::runtime_error("Invalid expression: use before load");
            }
            else if (dynamic_cast<AST::Ident*>(p->right) != nullptr)
            {
                AST::Ident* ident = dynamic_cast<AST::Ident*>(p->right);
                SymbolTable::IdEntry *e = p->pScope->idLookup(ident->value.getValue<std::string>());

                if (e != nullptr && ! e->loaded)
                    std::runtime_error("Invalid expression: use before load");
            }

            /** Free all registers used for expression */
            Register::Free();
            Register::Free();
            Register::Free();
        }
        else
        {
            std::cout << "Could not assign due to invalid memory location\n";
        }

        p->mem = mem;
        p->memName = tmp;
    }


    /**
     * Visitor Functions
     * 
     */

    void CodeGenVisitor::visit(AST::Assign *p)
    {
        std::cout << "Starting assignment\n";

        // visit left to find reg
        p->left->accept(this);
        // evaluate right hand side
        p->right->accept(this);

        // every sub expression saves output to memory location thus
        //  we need to load memory location from right into reg
        //  then we move reg into memory locaiton of left
        // therefor the following
        //
        //  lw reg, [right mem loc]
        //  sw reg, [left mem loc]

        if (p->left->mem != nullptr && p->right->mem != nullptr)
        {
            std::cout << "Writing comment: " << p->left->memName << " = " << p->right->memName << std::endl;
            // load right location
            emit(new Comment(p->left->memName + " = " + p->right->memName));
            Register *reg = Register::Next();
            emit("lw", reg, p->right->mem);
            addComment(new Comment("fill " + p->right->memName + " to " + reg->emit() + " from " + p->right->mem->emit()));
            emit("sw", reg, p->left->mem);
            addComment(new Comment("spill " + p->right->memName + " from " + reg->emit() + " to " + p->left->mem->emit()));

             if (dynamic_cast<AST::Ident*>(p->left) != nullptr)
            {
                AST::Ident* ident = dynamic_cast<AST::Ident*>(p->left);
                SymbolTable::IdEntry *e = p->pScope->idLookup(ident->value.getValue<std::string>());

                if (e != nullptr)
                    e->loaded = true;
            }

            Register::Free();
        }
        else
        {
            std::cout << "Could not assign due to invalid memory location\n";
        }
    }



    /**
     * @brief Visitor declaration and calculate offsets for expresions
     * 
     * @param p 
     */
    void CodeGenVisitor::visit(AST::Declaration *p)
    {
        std::cout << "Allocation offsets for declarations\n";

        SymbolTable::IdEntry* e= p->pScope->idLookup(p->ident.getValue<std::string>());

        if (e == nullptr)
            throw std::runtime_error("No declaration found for identifier: " + p->ident.getValue<std::string>());
        else
            std::cout << "Found declaration going to calculate offset\n";

        // if we are in function scope these are parameters
        std::string reg("fp");
        if (p->pScope->numOfParams > 0)
        {
            // block == param location
            // block is 0 indexed but first param starts at 4($fp)
            e->offset = (e->block+1) * 4;
        }
        // otherwise we are in global or child scope thus we get the offset
        else
        {
            e->offset =  - p->pScope->getNextOffset();

            if (e->block == 1)
                reg = "gp";
        }

        std::cout << "Setting offset to: " << e->offset << std::endl;
        std::cout << "Will calculate to: " << e->offset << "($" << reg <<")\n";
    }

    void CodeGenVisitor::visit(AST::StatementBlock *p)
    {
        std::cout << "Starting statement block code gen\n";

        for(auto decl : p->decls)
        {
            decl->accept(this);
        }

        std::cout << "Visiting statement body\n";

        for (auto stmt : p->stmts )
        {
            stmt->accept(this);
        }

    }


    void CodeGenVisitor::visit(AST::FunctionDeclaration *p)
    {
        std::string funcName = p->ident.getValue<std::string>();
        std::cout << "Staring gen of function: " << p->ident.getValue<std::string>() << std::endl;

        if (funcName.compare("main") == 0)
            emit(new Label(funcName));
        else
            emit(new Label("_" + funcName));

        // TODO figure out how to insert ref to offset size here
        emit("BeginFunc ", &(p->pScope->baseOffset) );
        p->pScope->baseOffset = 4;  // funciton base offset always starts at 4, first decl
        // will be allocated at -8($fp)

        // setup frame

        emit("subu", new Register("sp"), new Register("sp"), new Immediate("8"));
        instructions.back()->comment = new Comment("decrement sp to make space to save ra, fp");

        emit("sw", new Register("fp"), new Memory("sp", 8));
        instructions.back()->comment = new Comment("save fp");

        emit("sw", new Register("ra"), new Memory("sp", 8));
        instructions.back()->comment = new Comment("save ra");

        emit("addiu", new Register("fp"), new Register("sp"), new Immediate("8"));
        instructions.back()->comment = new Comment("set up new fp");

        // visit declarations to generate offsets for parameters (not needed in main)

        for(auto formal : p->formals )
        {
            formal->accept(this);
        }

        // get ref to current scopes space offset, this will be used later for instr
        //  output
        // TODO @ccs need to make Immediate value / location a ref so it's modifiable
        emit("subu", new Register("sp"), new Register("sp"), new Immediate(&(p->pScope->baseOffset)));
        instructions.back()->comment = new Comment("decrement sp to make space for locals/temps");

        emit("End frame setup");

        // generate sub expression
        emit("Statement Body");
        p->stmts->accept(this);


        // return from function
        emit("EndFunc");
        emit("(below handles reaching end of fn body with no explicit return)");
        
        emit("move", new Register("sp"), new Register("fp"));
        instructions.back()->comment = new Comment("pop callee frame off stack");

        emit("lw", new Register("ra"), new Memory("fp", -4));
        instructions.back()->comment = new Comment("restore saved ra");

        emit("lw", new Register("fp"), new Memory("fp", 0));
        instructions.back()->comment = new Comment("restore saved fp");

        emit("jr", new Register("ra"));
        instructions.back()->comment = new Comment("return from function");
    }

    void CodeGenVisitor::visit(AST::Program *p)
    {
        std::cout << "Beginning code gen of program with preamble\n";
        emit("standard Decaf preamble");
        emit(".text");
        emit(".align 2");
        emit(".globl main");

        std::cout << "Visiting declarations to assign memory locations\n";
        // TODO visit global variables

        std::cout << "Visiting functions for generation\n";
        for( auto func : p->func )
        {
            func->accept(this);
        }
    }
}