
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>


#include "ParseTree.hpp"

FormalVariableDeclaration::FormalVariableDeclaration(VariableDeclaration *var)
{
    if (var == nullptr)
        throw std::runtime_error("Invalid Formal");
    
    // var = var;
    ident = new Identifier();
    *ident = *var->ident;
    type = new DeclarationType();
    *type = *var->type;

}

PrintStmt::PrintStmt(CallExpression *expr)
{
    if (expr == nullptr)
        throw std::runtime_error("Invalid Expression");

    ident = new Identifier();
    *ident = *expr->ident;
    actuals.swap(expr->actuals);
    lparen = expr->lparen;
    rparen = expr->rparen;
}


std::string Identifier::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << std::setw(3) << ident.lineNumber
        << std::setw(numSpaces) << " " << extra
        << nodeName() << ident.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string DeclarationType::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;
    // we don't print number line for type
    ss  << std::setw(numSpaces + 3) << " " << extra 
        << nodeName() << type.getValue<std::string>() << std::endl;

    return ss.str();
}

std::string Declarations::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3) << ident->line() 
        << std::setw(numSpaces) << " "
        << extra << this->nodeName() << std::endl
        << type->toString(numSpaces + 3)
        << ident->toString(numSpaces + 3);

    return ss.str();
}

std::string LValue::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3) << ident->line()
        << std::setw(numSpaces) << " "
        << extra <<"FieldAccess:" << std::endl
        << ident->toString(numSpaces+3);

    return ss.str();
}

std::string Constant::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3) << constant.lineNumber
        << std::setw(numSpaces) << " "
        << extra << nodeName();
        
    if (constant.type == Scanner::Token::Type::DoubleConstant)
    {

        double integral = 0;
        double value = std::modf(constant.getValue<double>(), &integral);

        if (value == 0)
        {
            ss << constant.getValue<double>();
        }
        else
        {
            std::size_t decimal_index = constant.value.find('.');

            int decimal_places = constant.value.length() - decimal_index - 1;

            ss << std::fixed << std::setprecision((decimal_places < 1) ? 1 : decimal_places)
                << constant.getValue<double>();
        }



    }
    else if (constant.type == Scanner::Token::Type::IntConstant)
        ss << constant.getValue<int>();
    else
        ss << constant.getValue<std::string>();

    ss << std::endl;

    return ss.str();
}

std::string ParenExpr::toString(int numSpaces, std::string extra)
{
    return expr->toString(numSpaces, extra);
}


std::string Statement::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3) << line()
        << std::setw(numSpaces) << " "
        << extra << nodeName() << std::endl;

    return ss.str();
}

std::string Expression::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << Statement::toString(numSpaces, extra);
    
    if (expr != nullptr)
    {
        ss << expr->toString(numSpaces+3);
    }

    return ss.str();
}

std::string BinaryExpression::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << Expression::toString(numSpaces, extra)
        << std::setw(3) << line()
        << std::setw(numSpaces+3) << " "
        << "Operator: " << op.getValue<std::string>() << std::endl
        << right->toString(numSpaces+3);

    return ss.str();
}

std::string CallExpression::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << Statement::toString(numSpaces, extra);

    if (ident != nullptr)
    {
        ss << ident->toString(numSpaces+3);
    }

    for (auto actual : actuals)
    {
        ss << actual->toString(numSpaces+3, "(actuals) ");
    }

    return ss.str();
}

std::string PrintStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(numSpaces+3) << " "
        << extra <<nodeName() << std::endl;

    for (auto actual : actuals)
    {
        ss << actual->toString(numSpaces+3, "(args) ");
    }

    return ss.str();
}

std::string ReadIntExpr::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << Statement::toString(numSpaces, extra);

    return ss.str();
}

std::string ReadLineExpr::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << Statement::toString(numSpaces, extra);

    return ss.str();
}


std::string StatementBlock::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3+numSpaces) << " "
        << extra << nodeName() << std::endl;

    for(auto var : vars)
    {
        ss << var->toString(numSpaces+3);
    }

    for(auto stmt : stmts)
    {
        ss << stmt->toString(numSpaces+3);
    }

    return ss.str();
}

std::string FunctionDeclaration::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << Declarations::toString(numSpaces, extra);

    for (auto formal : formals)
    {
        ss << formal->toString(numSpaces + 3);
    }

    ss  << block->toString(numSpaces + 3, "(body) ");

    return ss.str();
}

std::string KeywordStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(3) << line()
        << std::setw(numSpaces) << " "
        << extra << nodeName() << std::endl;

    if (expr != nullptr) {
        ss << expr->toString(numSpaces+3);
    }

    return ss.str();
}

std::string ReturnStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << KeywordStmt::toString(numSpaces, extra);

    if (expr == nullptr && keyword.type == Scanner::Token::Type::Return)
    {
        ss << std::setw(3) << " "
            << std::setw(numSpaces+3) << " "
            << "Empty: " << std::endl;
    }

    return ss.str();
}

std::string WhileStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(numSpaces+3) << " "
        << nodeName() << std::endl
        << expr->toString(numSpaces+3, "(test) ");
    
    ss << stmt->toString(numSpaces+3, "(body) ");

    return ss.str();
}

std::string IfStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(numSpaces+3) << " "
        << extra << nodeName() << std::endl
        << expr->toString(numSpaces+3, "(test) ");

    ss << stmt->toString(numSpaces+3, "(then) ");

    if ( elseBlock != nullptr)
    {
        ss  << elseBlock->toString(numSpaces+3, "(else) ");
    }

    return ss.str();
}


std::string ForStmt::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss  << std::setw(numSpaces+3) << " "
        << extra << nodeName() << std::endl;
    
    if (startExpr != nullptr)
    {
        ss << startExpr->toString(numSpaces+3, "(init) ");
    } else 
    {
        ss  << std::setw(3+numSpaces+3) << " "
            << "(init) Empty:" << std::endl;
    }

    ss  << expr->toString(numSpaces+3,"(test) ");

    if (loopExpr != nullptr)
    {
        ss << loopExpr->toString(numSpaces+3, "(step) ");
    } else 
    {
        ss  << std::setw(3) << expr->line()
            << std::setw(numSpaces) << " "
            << "Empty: " << std::endl;
    }
    
    ss << stmt->toString(numSpaces+3, "(body) ");

    return ss.str();
}


std::string Program::toString(int numSpaces, std::string extra)
{
    std::stringstream ss;

    ss << std::setw(3) << " "
        << nodeName() << std::endl;

    for (auto decl : decls)
    {
        ss << decl->toString(numSpaces+3);
    }

    return ss.str();
}


