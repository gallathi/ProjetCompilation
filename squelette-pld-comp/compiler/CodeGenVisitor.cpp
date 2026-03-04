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
	
	// Prologue
	std::cout<< "    pushq %rbp\n";
	std::cout<< "    movq %rsp, %rbp\n";	

	// Parcours du code
	if (ctx->bloc() != nullptr) {
		visit(ctx->bloc());
	}
    visit(ctx->return_stmt());
    
    // Epilogue
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

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visit(ctx->expression());
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

antlrcpp::Any CodeGenVisitor::visitOpposite(ifccParser::OppositeContext *ctx) {
    visit(ctx->expression());
    std::cout << "    negl %eax" << std::endl;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx) {
	char op = ctx->op->getText()[0];
	visit(ctx->expression(0));
	std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	nextIndex += 4;
	visit(ctx->expression(1));
	if (op == '+') {
		std::cout << "    addl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	} else if (op == '-') {
		std::cout << "    subl %eax, -" << nextIndex - 4 << "(%rbp)" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	}
	nextIndex -= 4;
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx) {
	char op = ctx->op->getText()[0];
	visit(ctx->expression(0));
	std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	nextIndex += 4;
	visit(ctx->expression(1));
	if (op == '*') {
		std::cout << "    imul -" << nextIndex - 4 << "(%rbp)" << std::endl;
	} else if (op == '/') {
		std::cout << "    movl $0, %edx" << std::endl;
		std::cout << "    movl %eax, %ecx" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
		std::cout << "    idiv %ecx" << std::endl;
	} else if (op == '%') {
		std::cout << "    movl $0, %edx" << std::endl;
		std::cout << "    movl %eax, %ecx" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
		std::cout << "    idiv %ecx" << std::endl;
		std::cout << "    movl %edx, %eax" << std::endl;
	}
	nextIndex -= 4;
	return 0;
}
