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
	
	// prologue
	std::cout<< "    pushq %rbp\n";
	std::cout<< "    movq %rsp, %rbp\n";	
	
	if (ctx->bloc() != nullptr) {
		visit(ctx->bloc());
	}
    visit(ctx->return_stmt());
    
    // epilogue
    std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    std::string varName = ctx->VAR()->getText();
	int varIndex = varTable[varName].index;
	visit(ctx->expression());
	std::cout << "    movl %eax, -" << varIndex << "(%rbp)" << std::endl;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx) {
	std::cout << "    movl $" << ctx->CONST()->getText() << ", %eax" << std::endl;
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitVar(ifccParser::VarContext *ctx) {
	std::string varName = ctx->VAR()->getText();
	int varIndex = varTable[varName].index;
	std::cout << "    movl -" << varIndex << "(%rbp), %eax" << std::endl;
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visit(ctx->expression());
    return 0;
}
