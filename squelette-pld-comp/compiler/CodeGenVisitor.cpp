#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"

using namespace std;

CFG cfg;

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    cfg.gen_asm_prologue(cout);
    visit(ctx->block());
    cfg.gen_asm(cout);
    cfg.gen_asm_epilogue(cout);
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
	BasicBlock *bb = new BasicBlock(&cfg, cfg.new_BB_name());
	cfg.add_bb(bb);
	cfg.current_bb = bb;
	for (auto s : ctx->stmt())
	{
		visit(s);
	}
	visit(ctx->return_stmt());

	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	std::string varName = ctx->VAR()->getText();
	int varIndex = varTable[varName].index;
	string value = std::any_cast<string>(visit(ctx->expression()));
	cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {varName, value});

	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
	visit(ctx->expression());
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
	string tempVar = cfg.create_new_tempvar(Type::INT);
	string varName = ctx->CONST()->getText();
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, ctx->CONST()->getText()});
	return tempVar;
}

antlrcpp::Any CodeGenVisitor::visitVar(ifccParser::VarContext *ctx)
{
	string tempVar = cfg.create_new_tempvar(Type::INT);
	string varName = ctx->VAR()->getText();
	cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {tempVar, varName});

	return tempVar;
}

antlrcpp::Any CodeGenVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
{
	visit(ctx->expression());
	std::cout << "    negl %eax" << std::endl;
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
	char op = ctx->op->getText()[0];
	visit(ctx->expression(0));
	std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	nextIndex += 4;
	visit(ctx->expression(1));
	if (op == '+')
	{
		std::cout << "    addl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	}
	else if (op == '-')
	{
		std::cout << "    subl %eax, -" << nextIndex - 4 << "(%rbp)" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	}
	nextIndex -= 4;
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
	char op = ctx->op->getText()[0];
	visit(ctx->expression(0));
	std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	nextIndex += 4;
	visit(ctx->expression(1));
	if (op == '*')
	{
		std::cout << "    imull -" << nextIndex - 4 << "(%rbp)" << std::endl;
	}
	else if (op == '/')
	{
		std::cout << "    movl $0, %edx" << std::endl;
		std::cout << "    movl %eax, %ecx" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
		std::cout << "    idiv %ecx" << std::endl;
	}
	else if (op == '%')
	{
		std::cout << "    movl $0, %edx" << std::endl;
		std::cout << "    movl %eax, %ecx" << std::endl;
		std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
		std::cout << "    idiv %ecx" << std::endl;
		std::cout << "    movl %edx, %eax" << std::endl;
	}
	nextIndex -= 4;
	return antlrcpp::Any();
}
