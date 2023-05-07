
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

    int Label::counter = 0;

    Label * Label::Next()
    {
        std::stringstream ss;

        ss << "_L" << counter ++;

        return new Label(ss.str());
    }

    Label * Label::Next(std::string info)
    {
        std::stringstream ss;

        ss << "_L" << info << counter ++;

        return new Label(ss.str());

    }

    std::string Label::emit()
    {
        std::stringstream ss;

        ss << label << ": ";

        if (comment != nullptr)
            ss << comment->emit() << " ";

        return label;
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

    // Using three registers for FP 
    int FloatingRegister::registerCount = 3;
    int FloatingRegister::registerIndex = 0;


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


    FloatingRegister::FloatingRegister()
    {

    }

    // do I need this
    FloatingRegister::FloatingRegister(std::string reg)
        : name(reg)
    {

    }

    std::string FloatingRegister::emit()
    {
        std::stringstream ss;
        
        ss << "$" << name;

        return ss.str();
    }

    FloatingRegister* FloatingRegister::Next()
    {
        registerIndex = (registerIndex + 1)%registerCount;
        std::stringstream ss;
        // Multiply index by two since we will be using double precision
        // 0 * 2 = 0,1  first double precision value
        // 1 * 2 = 2,3  second
        // 2 * 2 = 4,5  third
        ss << "f" << registerIndex*2;  
        return new FloatingRegister(ss.str());
    }

    void FloatingRegister::Free()
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
            ss << comment->emit() << " ";

        return ss.str();
    }

    Instruction::Instruction() 
        : op()
        , operand1(nullptr)
        , operand2(nullptr)
        , operand3(nullptr)
    {

    }

    Instruction::Instruction(std::string op, Location* op1)
        : op(op)
        , operand1(op1)
        , operand2(nullptr)
        , operand3(nullptr)
    {

    }

    Instruction::Instruction(std::string op, Location* op1, 
        Location* op2)
        : op(op)
        , operand1(op1)
        , operand2(op2)
        , operand3(nullptr)
    {

    }
    
    Instruction::Instruction(std::string op, Location* op1, 
        Location* op2, Location* op3)
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
        , labelCounter(1)
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

    void CodeGenVisitor::emit(std::string op, Location* operand1, 
        Location* operand2)
    {
        instructions.push_back(
                new Instruction(op, operand1, operand2) );
    }

    void CodeGenVisitor::emit(std::string op, Location* operand1, 
        Location* operand2, Location* operand3)
    {
        instructions.push_back(
            new Instruction(op, operand1, operand2, operand3));
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

        // std::cout << "Opening file with name: " << actual_file_name << std::endl;
        std::ofstream file;
        file.open( actual_file_name );

        for( auto instrs : instructions )
        {
            
            std::string spacing("\t");
            if (dynamic_cast<Label*>(instrs) != nullptr )
            {
                spacing = "  ";
            }

            std::string instr(instrs->emit());
            file    << spacing
                    << instr;

            if (dynamic_cast<Label*>(instrs) != nullptr )
            {
                spacing = "  ";
                file << ": ";
                if ( instrs->comment != nullptr )
                    file << instrs->comment->emit();
            }

            file << std::endl;

        }

        file.close();
    }

    void CodeGenVisitor::loadSubExpr(AST::Node *p, Register *reg)
    {
        switch(p->outType)
        {
            case Scanner::Token::Type::Double:
                emit("l.d", reg, p->mem);
                break;
            default:
                emit("lw", reg, p->mem);
        }
        addComment(new Comment("fill " + p->memName + " to " + reg->emit() + " from " + p->mem->emit()));
    }

    void CodeGenVisitor::loadSubExprs(AST::Node *left, 
        Register *lreg, AST::Node *right, Register *rreg)
    {
        loadSubExpr(left, lreg);
        loadSubExpr(right, rreg);
    }

    void CodeGenVisitor::saveSubExpr(AST::Node *p, Register *reg, 
        Memory* mem, std::string tmpName)
    {
        switch(p->outType)
        {
            case Scanner::Token::Type::Double:
                emit("s.d", reg, mem);
                break;
            default:
                emit("sw", reg, mem);
        }
        addComment(new Comment("spill " + tmpName + " from " + reg->emit() + " to " + mem->emit()));
    }

    Memory * CodeGenVisitor::setupSubExpr(AST::Node *left, 
        AST::Node *right, SymbolTable::Scope *pScope, 
        std::string &tmpName)
    {
        left->accept(this);

        if (right != nullptr)
            right->accept(this);

        int offset = pScope->getNextOffset();

        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;
        tmpName = ss.str();
        
        return new Memory("fp", -offset);
    }

    void CodeGenVisitor::identCheck(AST::Node *p, 
        SymbolTable::Scope *pScope)
    {
        if (dynamic_cast<AST::Ident*>(p) != nullptr)
        {
            AST::Ident* ident = dynamic_cast<AST::Ident*>(p);
            SymbolTable::IdEntry *e = pScope->idLookup(ident->value.getValue<std::string>());

            if (e != nullptr && ! e->loaded)
                std::runtime_error("Invalid expression: use before load");
        }
            
    }

    void CodeGenVisitor::unaryExpr(AST::Expr *p, std::string op)
    {
        std::string tmp("");

        Memory *mem = setupSubExpr(p->left, p->right, p->pScope, tmp);

        int start( p->minCol() );
        int end( p->maxCol() );

        if (p->left->mem != nullptr)
        {
            Register *lreg = Register::Next();
            Register *oreg = Register::Next();

            if (p->left->outType == Scanner::Token::Type::Double)
            {
                lreg = FloatingRegister::Next();
                oreg = FloatingRegister::Next();

                int offset = p->pScope->getNextOffset(); // get next offset to save double
                mem = new Memory("fp", -offset);
            }

            emit(new Comment( tmp + " = " + p->op.lineInfo.substr(start-2, end-start+2)) ); // expression start
            loadSubExpr(p->left, lreg);

            emit(op, oreg, lreg);

            saveSubExpr(p, oreg, mem, tmp);

            identCheck(p->left, p->pScope);

            if (p->left->outType == Scanner::Token::Type::Double)
            {
                FloatingRegister::Next();
                FloatingRegister::Next();
            }
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

    void CodeGenVisitor::binaryExpr(AST::Expr *p, std::string op)
    {
        std::string tmp("");

        Memory *mem = setupSubExpr(p->left, p->right, p->pScope, tmp);

        int start( p->minCol() );
        int end( p->maxCol() );

        if (p->left->mem != nullptr && p->right->mem != nullptr)
        {
            // load right location
            Register *rreg = Register::Next();
            Register *lreg = Register::Next();
            Register *oreg = Register::Next();

            if (p->left->outType == Scanner::Token::Type::Double)
            {
                rreg = FloatingRegister::Next();
                lreg = FloatingRegister::Next();
                oreg = FloatingRegister::Next();

                int offset = p->pScope->getNextOffset(); // get next offset to save double
                mem = new Memory("fp", -offset);
            }

            emit(new Comment( tmp + " = " + p->op.lineInfo.substr(start-2, end-start+2)) ); // expression start
            loadSubExprs(p->left, lreg, p->right, rreg);
            // add instr
            emit(op, oreg, lreg, rreg);
            
            // save to mem
            saveSubExpr(p, oreg, mem, tmp);
            // add instr

            identCheck(p->left, p->pScope);
            identCheck(p->right, p->pScope);

            /** Free all registers used for expression */
            if (p->left->outType == Scanner::Token::Type::Double)
            {
                FloatingRegister::Next();
                FloatingRegister::Next();
                FloatingRegister::Next();
            }
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

    void CodeGenVisitor::floatingPointLogical(AST::Expr *p, std::string op, 
        bool invert, bool equalCheck)
    {
        // visit left/right
        std::string tmp( "" );
        Memory *mem = setupSubExpr(p->left, p->right, p->pScope, tmp);
        
        // Don't need to get extra offset as output is 32-bit value
        // int offset = p->pScope->getNextOffset();
        // mem = new Memory("fp", -offset);

        // Load FP regs
        FloatingRegister *lreg = FloatingRegister::Next();
        FloatingRegister *rreg = FloatingRegister::Next();

        loadSubExpr(p->left, lreg);
        loadSubExpr(p->right, rreg);
        
        // FP compare a < b
        emit(op, lreg, rreg);

        Label *fbranch = Label::Next();
        Label *ebranch = Label::Next();

        // Branch on FP compare if false
        emit("bclf", fbranch);
        
        // get normal register
        Register *oreg = Register::Next();
        Immediate *immFbranch = new Immediate("0");
        Immediate *immTbranch = new Immediate("1");
        if (invert)
        {
            // swap values
            immFbranch->immediate = "1";
            immTbranch->immediate = "0";
        }
        
        
        // Branch True
        // certain comparison like lessthan/greaterthan require additional 
        //  equality check
        if (equalCheck)
        {
            // in expressions like a < b OR a > b
            // If values are equal than we need to return false
            emit("c.eq.d", lreg, rreg);
            emit("bclf", fbranch);
        
        }
        // li $t*, 1    // return 1
        emit("li", oreg, immTbranch);
        // sw $t*, out memory
        emit("sw", oreg, mem);
        // branch end
        emit("b", ebranch);

        // Branch False
        emit(fbranch);
        // li $t*, 0    // return 0
        emit("li", oreg, immFbranch);
        // sw $t*, out memory
        emit("sw", oreg, mem);
        // branch end

        // Branch end
        emit(ebranch);

        Register::Free();

        p->mem = mem;
        p->memName = tmp;
    }

    void CodeGenVisitor::pushParam(AST::Node *p)
    {
        Register *reg = Register::Next();

        if (p->outType == Scanner::Token::Type::Double)
            reg = FloatingRegister::Next();

        emit(new Comment("PushParam " + p->memName));

        // allocate space on stack for param
        Immediate *imm = new Immediate("4");
        if (p->outType == Scanner::Token::Type::Double )
            imm->immediate = "8";

        emit("subu", new Register("sp"), new Register("sp"), imm);
        addComment(new Comment("decrement sp to make space for param"));

        // load parameter into reg
        loadSubExpr(p, reg);
        // emit("lw", reg, p->mem);
        // addComment(new Comment("fill " + p->memName + " to " + reg->emit() + " from " + p->mem->emit()));

        // push parameter onto stack for call 
        Memory *mem = new Memory("sp", 4);
        if (p->outType == Scanner::Token::Type::Double )
            mem->offset += 4;

        saveSubExpr(p, reg, mem, p->memName);
        // emit("sw", reg, new Memory("sp", 4));
        // addComment(new Comment("copy param value to stack"));
        
        if (p->outType == Scanner::Token::Type::Double)
            FloatingRegister::Free();

        Register::Free();
    }

    void CodeGenVisitor::popParams(int num)
    {
        std::stringstream ss;
        ss << "PopParams " << num*4;

        std::stringstream ss2;
        ss2 << num*4;

        emit(new Comment(ss.str()));
        emit("add", new Register("sp"), new Register("sp"), new Immediate(ss2.str()));
        addComment(new Comment("pop params off stack"));
    }

    void CodeGenVisitor::CallFormalVisit(AST::Call *p)
    {
        for( auto it = p->actuals.cbegin(); it != p->actuals.cend(); ++it)
        {
            AST::Node *formal( *it );

            formal->accept(this);
        }
    }

    void CodeGenVisitor::CallFormalPush(AST::Call *p)
    {
        for( auto it = p->actuals.crbegin(); it != p->actuals.crend(); ++it)
        {
            AST::Node *formal( *it );

            pushParam( formal );
        }
    }

    void CodeGenVisitor::saveReturn(AST::Call *p, std::string tmpName)
    {
        // get new temporary offset
        int offset = p->pScope->getNextOffset();
        Memory *mem = new Memory("fp", -offset);
        Register *reg = Register::Next();

        if (p->outType == Scanner::Token::Type::Double)
        {
            offset = p->pScope->getNextOffset();
            mem = new Memory("fp", -offset);

            // Return Value in $f6
            reg = new FloatingRegister("f6");

            saveSubExpr(p, reg, mem, tmpName);
        }
        else
        {
            emit("move", reg, new Register("v0"));
            addComment(new Comment("copy function return value from $v0"));

            saveSubExpr(p, reg, mem, tmpName);
            // emit("sw", reg, mem);
            // addComment(new Comment("spill " + tmpName + " from " + reg->emit() + " to " + mem->emit()));

        }

        Register::Free();

        p->mem = mem;
        p->memName = tmpName;

    }

    void CodeGenVisitor::functionReturn()
    {
        emit("move", new Register("sp"), new Register("fp"));
        addComment(new Comment("pop callee frame off stack"));

        emit("lw", new Register("ra"), new Memory("fp", -4));
        addComment(new Comment("restore saved ra"));

        emit("lw", new Register("fp"), new Memory("fp", 0));
        addComment(new Comment("restore saved fp"));

        emit("jr", new Register("ra"));
        addComment(new Comment("return from function"));
    }










    void CodeGenVisitor::visit(AST::Ident *p)
    {
        // lookup in symbol table to retrieve register
        SymbolTable::IdEntry *e = p->pScope->idLookup(p->value.getValue<std::string>());

        p->reg = e->reg;

        std::string reg("fp");

        if (e->block == 1)
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

        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;
        std::string tmp(ss.str());
        std::string constant(p->value.getValue<std::string>());

        emit(new Comment(tmp + " = " + constant));

        Register *reg = Register::Next(); //get next register

        switch (p->value.type)
        {
            case Scanner::Token::Type::StringConstant:
                {
                    emit(".data");
                    addComment(new Comment("create string constant marked with label"));
                    std::stringstream ss;
                    ss << "_string" << labelCounter++;
                    Label *l = new Label(ss.str());
                    
                    ss.str("");
                    ss << l->emit() << ": .asciiz " << constant ;

                    emit(ss.str());
                    emit(".text");
                    emit("la", reg, l);
                    addComment(new Comment("load label"));
                }
                break;
            case Scanner::Token::Type::DoubleConstant:
                // must get new offset for floating point value
                reg = FloatingRegister::Next();
                offset = p->pScope->getNextOffset();
                mem = new Memory("fp", -offset);
                emit("li.d", reg, new Immediate(constant));
                addComment(new Comment("load constant value " + constant + " into " + reg->emit() ));
                break;
            case Scanner::Token::Type::BoolConstant :
                if (constant.compare("true") == 0)
                {
                    constant = std::string("1");
                }
                else
                {
                    constant = std::string("0");
                }
                // set constant "true" or "false" to int value so fallback to default emit
            default :
                emit("li", reg, new Immediate(constant));
                addComment(new Comment("load constant value " + constant + " into " + reg->emit() ));
                break;

        }

        // Each constant will be saved to memory locaiton
        if (p->value.type == Scanner::Token::Type::DoubleConstant)
        {
            emit("s.d", reg, mem);
            FloatingRegister::Free();
        }
        else
        {
            emit("sw", reg, mem);
        }
        Register::Free();
        addComment(new Comment("spill " + tmp + " from " + reg->emit() + " to " + mem->emit()));

        p->mem = mem;
        p->memName = tmp;
    }

    // Arithemetic Expressions

    void CodeGenVisitor::visit(AST::Modulus *p)
    {
        std::string op( "rem" );
        binaryExpr(p, "rem");
    }

    void CodeGenVisitor::visit(AST::Multiply *p)
    {
        std::string op( "mul" );
        if (p->outType == Scanner::Token::Type::Double)
            op += ".d";
        binaryExpr(p, op);
    }

    void CodeGenVisitor::visit(AST::Divide *p)
    {
        std::string op( "div" );
        if (p->outType == Scanner::Token::Type::Double)
            op += ".d";
        binaryExpr(p, op);
    }

    void CodeGenVisitor::visit(AST::Subtract *p)
    {
        // have to be careful as this might be unary
        std::string op( "sub" );
        
        if (p->right != nullptr)
        {
            if (p->outType == Scanner::Token::Type::Double)
                op += ".d";
            binaryExpr(p, op);
        }
        else
        {
            op = "neg";
            if (p->outType == Scanner::Token::Type::Double)
                op += ".d";
            // unary expression
            unaryExpr(p, op);
        }
    }

    void CodeGenVisitor::visit(AST::Add *p)
    {
        std::string op( "add" );
        if (p->outType == Scanner::Token::Type::Double)
            op += ".d";
        binaryExpr(p, op);
    }


    // Logical Expression

    // And/Or will always be done on bool values in normal registers
    void CodeGenVisitor::visit(AST::And *p)
    {
        binaryExpr(p, "and");
    }

    void CodeGenVisitor::visit(AST::Or *p)
    {
        binaryExpr(p, "or");
    }

    // Comparison will operate on normal registers for integers/strings/bools
    //  For doubles will need branches to check coprocess flag and load bool
    //  value into memory
    void CodeGenVisitor::visit(AST::GTE *p)
    {
        // Have to check left/right type, logical operators outType is always bool
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "sge");
        }
        else
        {
            // invert op result
            floatingPointLogical(p, "c.lt.d", true, false);
        }
    }
    
    void CodeGenVisitor::visit(AST::GreaterThan *p)
    {
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "sgt");
        }
        else
        {
            // invert op results and perform additional equal check
            floatingPointLogical(p, "c.lt.d", true, true);
        }
    }

    void CodeGenVisitor::visit(AST::LTE *p)
    {
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "sle");
        }
        else
        {
            floatingPointLogical(p, "c.le.d", false, false);
        }
    }

    void CodeGenVisitor::visit(AST::LessThan *p)
    {
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "slt");
        }
        else
        {
            floatingPointLogical(p, "c.lt.d", false, false);
        }
    }

    void CodeGenVisitor::visit(AST::Equal *p)
    {
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "seq");
        }
        else
        {
            floatingPointLogical(p, "c.eq.d", false, false);
        }
    }

    void CodeGenVisitor::visit(AST::NotEqual *p)
    {
        if (p->left->outType != Scanner::Token::Type::Double)
        {
            binaryExpr(p, "sne");
        }
        else
        {
            // invert equality check
            floatingPointLogical(p, "c.eq.d", true, false);
        }
    }

    void CodeGenVisitor::visit(AST::Not *p)
    {
        // unaryExpr(p, "not");
        // Not is special, we will visit expression
        //  then compare output to 0 this will result in logical not
        // Example:
        //  bool == 0
        //  1 == 0  = 0
        //  0 == 0  = 1
        p->left->accept( this );

        Register *reg = Register::Next();
        Register *lvalue = Register::Next();
        Register *outValue = Register::Next();
        int offset = p->pScope->getNextOffset();
        Memory *mem = new Memory("fp", -offset);

        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;
        std::string tmp(ss.str());

        emit("li", reg, new Immediate("0"));
        addComment(new Comment("load constant value '0' into " + reg->emit() ));

        emit("lw", lvalue, p->left->mem);
        emit("seq", outValue, lvalue, reg);

        emit("sw", outValue, mem);

        p->mem = mem;
        p->memName = tmp;

        Register::Free();
        Register::Free();
        Register::Free();

    }


    /**
     * Visitor Functions
     * 
     */

    void CodeGenVisitor::visit(AST::Assign *p)
    {
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
            // load right location
            emit(new Comment(p->left->memName + " = " + p->right->memName));
            
            Register *reg = Register::Next();

            if (p->outType == Scanner::Token::Type::Double)
                reg = FloatingRegister::Next();
            
            loadSubExpr(p->right, reg);
            // emit("lw", reg, p->right->mem);
            // addComment(new Comment("fill " + p->right->memName + " to " + reg->emit() + " from " + p->right->mem->emit()));

            saveSubExpr(p, reg, p->left->mem, p->left->memName);
            // emit("sw", reg, p->left->mem);
            // addComment(new Comment("spill " + p->right->memName + " from " + reg->emit() + " to " + p->left->mem->emit()));
            
            identCheck(p->left, p->pScope);

            if (p->outType == Scanner::Token::Type::Double)
                FloatingRegister::Free();

            Register::Free();
        }
        else
        {
            std::cout << "Could not assign due to invalid memory location\n";
        }
    }


    // Keyword Visitors
    void CodeGenVisitor::visit(AST::Return *p)
    {
        if (p->expr != nullptr)
        {
            p->expr->accept(this);
            emit(new Comment("Return " + p->expr->memName));

            Register *reg;

            if ( p->outType == Scanner::Token::Type::Double )
            {
                reg = FloatingRegister::Next();
                emit("l.d", reg, p->expr->mem);
                addComment(new Comment("fill " + p->expr->memName + " to " + reg->emit() + " from " + p->expr->mem->emit()));
                
                FloatingRegister * outreg = new FloatingRegister("f6");
                // TODO need to save FP register to specific reg on return maybe fp6 ?
                emit("mov.d", outreg, reg);

                FloatingRegister::Free();
            }
            else
            {
                reg = Register::Next();
                emit("lw", reg, p->expr->mem);
                addComment(new Comment("fill " + p->expr->memName + " to " + reg->emit() + " from " + p->expr->mem->emit()));
                emit("move", new Register("v0"), reg);
                addComment(new Comment("assign return value into $v0"));
                Register::Free();
            }
        }
        else
        {
            emit("Return ");
        }
        
        functionReturn();
    }

    void CodeGenVisitor::visit(AST::Break *p)
    {
        emit("b", endLoop);
    }

    void CodeGenVisitor::visit(AST::If *p)
    {
        // Get Else and End Label
        Label * elseLabel = Label::Next();
        Label * endLabel = Label::Next();

        if (p->elseStmt == nullptr)
            elseLabel = endLabel;

        // Get reg to hold conditional value
        Register *reg = Register::Next();

        p->expr->accept(this);

        emit(new Comment("IfZ " + p->expr->memName + " Goto " + elseLabel->emit()));

        emit("lw", reg, p->expr->mem);
        emit("beqz", reg, elseLabel);

        Register::Free();

        // Emit statment Body
        p->stmt->accept(this);

        // Else Block
        if (p->elseStmt != nullptr)
        {
            // only need to emit branch if else block is present
            emit(new Comment("Goto " + endLabel->emit()));
            emit("b", endLabel);
            emit(elseLabel);

            p->elseStmt->accept(this);
        }

        // Emit end label
        emit(endLabel);
    }

    void CodeGenVisitor::visit(AST::While *p)
    {
        // Get Start of Loop Label and End Label
        Label *start = Label::Next();
        endLoop = Label::Next();

        // emit start label
        emit(start);

        // Get reg to hold conditional value
        Register *reg = Register::Next();

        p->expr->accept(this);

        emit(new Comment("IfZ " + p->expr->memName + " Goto " + endLoop->emit()));

        emit("lw", reg, p->expr->mem);
        emit("beqz", reg, endLoop);

        Register::Free();

        // statement body
        p->stmt->accept(this);

        emit("b", start);   // branch back to start of loop

        // emit end of loop
        emit(endLoop);
    }

    void CodeGenVisitor::visit(AST::For *p)
    {
        if (p->startExpr != nullptr)
            p->startExpr->accept(this);

        Label *start = Label::Next();
        endLoop = Label::Next();

        emit(start);
        // Get reg to hold conditional value
        Register *reg = Register::Next();

        p->expr->accept(this);

        emit(new Comment("IfZ " + p->expr->memName + " Goto " + endLoop->emit()));

        emit("lw", reg, p->expr->mem);
        emit("beqz", reg, endLoop);

        Register::Free();

        // statement body
        p->stmt->accept(this);

        if (p->loopExpr != nullptr)
            p->loopExpr->accept(this);

        emit("b", start);   // branch back to start of loop

        emit(endLoop);

    }


    // Call visitors



    void CodeGenVisitor::visit(AST::Call *p)
    {
        CallFormalVisit(p);
        CallFormalPush(p);

        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;

        std::string tmp(ss.str());

        ss.str("");
        ss << "_" << p->value.getValue<std::string>();

        Label* l = new Label(ss.str());

        emit(new Comment(tmp + " = " + l->emit()));

        emit("jal", l);
        addComment(new Comment("jump to function"));

        saveReturn(p, tmp);

        int size = 0;
        for ( auto &p : p->actuals)
        {
            size++;
            // We allocate 8 on stack instead of 4 for doubles
            // easy hack is to pretend like we have 1 more parameter
            // for each double in the parameter list
            if (p->outType == Scanner::Token::Type::Double)
                size++;
        }

        popParams(size);
    }


    void CodeGenVisitor::visit(AST::Print *p)
    {
        // Don't need to worry about doubles here
        CallFormalVisit(p);

        // Print builtin only accepts single argument based on type
        // after each parameter push we call the corresponding funciton
        for( auto it = p->actuals.cbegin(); it != p->actuals.cend(); ++it)
        {
            AST::Node *formal( *it );

            formal->accept( this );

            // push parameter onto stack
            pushParam( formal );

            Label *l( nullptr ); // label to print call
            switch ( formal->outType )
            {
            case Scanner::Token::Type::Int :
                l = new Label("_PrintInt");
                break;
            case Scanner::Token::Type::String :
                l = new Label("_PrintString");
                break;
            case Scanner::Token::Type::Bool :
                l = new Label("_PrintBool");
            default:
                break;
            }

            emit(new Comment("LCall " + l->label));
            emit("jal", l);
            addComment(new Comment("jump to function"));

            // now we need to pop params
            popParams(1);
        }

    }

    void CodeGenVisitor::visit(AST::ReadLine *p)
    {
        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;

        Label* l = new Label("_ReadLine");

        emit(new Comment(ss.str() + " = " + l->emit()));

        emit("jal", l);
        addComment(new Comment("jump to function"));

        saveReturn(p, ss.str());
    }

    void CodeGenVisitor::visit(AST::ReadInteger *p)
    {
        std::stringstream ss;
        ss << "_tmp" << tmpCounter++;

        Label* l = new Label("_ReadInteger");

        emit(new Comment(ss.str() + " = " + l->emit()));

        emit("jal", l);
        addComment(new Comment("jump to function"));

        saveReturn(p, ss.str());

    }


    /**
     * @brief Visitor declaration and calculate offsets for expresions
     * 
     * @param p 
     */
    void CodeGenVisitor::visit(AST::Declaration *p)
    {
        SymbolTable::IdEntry* e= p->pScope->idLookup(p->ident.getValue<std::string>());

        if (e == nullptr)
            throw std::runtime_error("No declaration found for identifier: " + p->ident.getValue<std::string>());

        // if we are in function scope these are parameters
        std::string reg("fp");
        if (p->pScope->numOfParams > 0)
        {
            // block == param location
            // block is 0 indexed but first param starts at 4($fp)
            
            // TODO need to handle if double, but really only if previous decl is double?
            // e->offset = (e->paramIndex+1) * 4;
            e->offset = p->pScope->getNextParamOffset(); // will this work?

            // If this is a double we need to move offset twice
            if (e->type == Scanner::Token::Type::Double)
                p->pScope->getNextParamOffset();

            // because stack grows downward the first offset is the start of the 
            // double, the second address is in the middle, thus we do not need it
        }
        // otherwise we are in global or child scope thus we get the offset
        else
        {
            e->offset =  - p->pScope->getNextOffset();

            // if double we need to offset again for double precision
            if (e->type == Scanner::Token::Type::Double)
                e->offset = - p->pScope->getNextOffset();

            if (e->block == 1)
                reg = "gp";
        }

        // std::cout << "Setting offset to: " << e->offset << std::endl;
        // std::cout << "Will calculate to: " << e->offset << "($" << reg <<")\n";
    }

    void CodeGenVisitor::visit(AST::StatementBlock *p)
    {
        for(auto decl : p->decls)
        {
            decl->accept(this);
        }
        
        for (auto stmt : p->stmts )
        {
            stmt->accept(this);
        }

    }


    void CodeGenVisitor::visit(AST::FunctionDeclaration *p)
    {
        std::string funcName = p->ident.getValue<std::string>();
        // std::cout << "Staring gen of function: " << p->ident.getValue<std::string>() << std::endl;

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

        emit("sw", new Register("ra"), new Memory("sp", 4));
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

        functionReturn();
    }

    void CodeGenVisitor::visit(AST::Program *p)
    {
        emit("standard Decaf preamble");
        emit(".text");
        emit(".align 2");
        emit(".globl main");

        for( auto var : p->vars)
        {
            var->accept(this);
        }

        for( auto func : p->func )
        {
            func->accept(this);
        }
    }
}