#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    #ifdef __APPLE__
    std::cout<< ".globl _main\n" ;
    std::cout<< " _main: \n" ;
    #else
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;
    #endif

    std::cout << "    pushq %rbp\n";
    std::cout << "    movq %rsp, %rbp\n";

    this->visit( ctx->bloc());
    this->visit( ctx->return_stmt() );
    
    std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    std::string var = ctx->VAR()->getText();
    visit(ctx->expression());
    int position = varVisitor->getPosition(var);
    std::cout << "    movl %eax, -" << position << "(%rbp)\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitVar(ifccParser::VarContext *ctx)
{
    std::string var = ctx->VAR()->getText();
    int position = varVisitor->getPosition(var);
    std::cout << "    movl -" << position << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx) {
    std::cout << "    movl $" << ctx->CONST()->getText() << ", %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visit(ctx->expression());
    return 0;
}
