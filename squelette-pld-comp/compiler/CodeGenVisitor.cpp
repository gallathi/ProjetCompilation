#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"
#include <stack>

using namespace std;

CFG cfg;
bool debug_cfg = true;

string cond = "a";

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
	BasicBlock *bb = new BasicBlock(&cfg, cfg.new_BB_name());
	cfg.add_bb(bb);
	cfg.current_bb = bb;

	for (const auto &[name, info] : varTable)
	{
		cfg.add_to_symbol_table(name, Type::INT);
	}

	// cfg.gen_asm_prologue(cout);
	visit(ctx->block());
	// cfg.gen_asm(cout);
	// cfg.gen_asm_epilogue(cout);

	if (debug_cfg)
		cout << cfg;

	visit(ctx->block());
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{

	for (auto s : ctx->stmt())
	{
		visit(s);
	}

	return antlrcpp::Any();
}

std::stack<BasicBlock *> endifStack;

antlrcpp::Any CodeGenVisitor::visitElse_if(ifccParser::Else_ifContext *ctx)
{
	BasicBlock *elseBB = new BasicBlock(&cfg, cfg.new_BB_name());

	elseBB->test_var_name = cond;
	cond[0]++;

	cfg.add_bb(elseBB);
	elseBB->exit_true = endifStack.top();

	cfg.current_bb->exit_false = elseBB;

	cfg.current_bb = elseBB;
	visit(ctx->block());
	cfg.current_bb->exit_true = endifStack.top();

	// cout << "else_if";

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitElse(ifccParser::ifccParser::ElseContext *ctx)
{

	BasicBlock *elseBB = new BasicBlock(&cfg, cfg.new_BB_name());

	cfg.add_bb(elseBB);
	elseBB->exit_true = endifStack.top();

	cfg.current_bb->exit_false = elseBB;

	cfg.current_bb = elseBB;
	visit(ctx->block());
	cfg.current_bb->exit_true = endifStack.top();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitConditional(ifccParser::ConditionalContext *ctx)
{
	bool entry = 0;

	BasicBlock *endifBB = new BasicBlock(&cfg, cfg.new_BB_name());
	endifStack.push(endifBB);

	cfg.current_bb->test_var_name = cond;
	cond[0]++;

	BasicBlock *thenBB = new BasicBlock(&cfg, cfg.new_BB_name());
	thenBB->exit_true = endifStack.top(); // unconditional jump to endif at the end of the then block
	cfg.add_bb(thenBB);

	// the parent block
	cfg.current_bb->exit_true = thenBB;
	cfg.current_bb->exit_false = endifStack.top(); // will be changed by next block if there is an else if or else

	// cfg.current_bb = endifBB; // default end of graph, will change if there is another conditionals

	for (auto s : ctx->else_stmt())
	{
		visit(s);
	}

	cfg.current_bb = thenBB; // if true block
	visit(ctx->block());

	// n'importe quoi qui vient aprés le if, il doit pointer vers le endif final
	cfg.current_bb->exit_true = endifStack.top(); // jump inconditionelle

	cfg.add_bb(endifStack.top());
	cfg.current_bb = endifStack.top(); // the rest of the program will add to the endif block

	endifStack.pop();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	std::string varName = ctx->VAR()->getText();
	int varIndex = varTable[varName].index;
	string value = std::any_cast<string>(visit(ctx->expression()));
	cfg.add_to_symbol_table(varName, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {varName, value});

	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
	string value = std::any_cast<string>(visit(ctx->expression()));
	cfg.current_bb->add_IRInstr(IRInstr::return_instr, Type::INT, {value});
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
	string tempVar = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(tempVar, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, ctx->CONST()->getText()});

	return tempVar;
}

antlrcpp::Any CodeGenVisitor::visitVar(ifccParser::VarContext *ctx)
{
	string varName = ctx->VAR()->getText();
	return varName;
}

antlrcpp::Any CodeGenVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
{

	string rhs = std::any_cast<string>(visit(ctx->expression())); // operand result
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::neg, Type::INT, {out, rhs});
	return out;
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{

	// char op = ctx->op->getText()[0];
	// visit(ctx->expression(0));
	// std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	// nextIndex += 4;
	// visit(ctx->expression(1));
	// if (op == '+')
	//{
	//	std::cout << "    addl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	// }
	// else if (op == '-')
	//{
	//	std::cout << "    subl %eax, -" << nextIndex - 4 << "(%rbp)" << std::endl;
	//	std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	// }
	// nextIndex -= 4;
	// return antlrcpp::Any();

	char op = ctx->op->getText()[0];
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	if (op == '+')
	{
		cfg.current_bb->add_IRInstr(IRInstr::add, Type::INT, {out, lhs, rhs});
	}
	else if (op == '-')
	{
		cfg.current_bb->add_IRInstr(IRInstr::sub, Type::INT, {out, lhs, rhs});
	}
	return out;
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{

	// char op = ctx->op->getText()[0];
	// visit(ctx->expression(0));
	// std::cout << "    movl %eax, -" << nextIndex << "(%rbp)" << std::endl;
	// nextIndex += 4;
	// visit(ctx->expression(1));
	// if (op == '*')
	//{
	//	std::cout << "    imull -" << nextIndex - 4 << "(%rbp)" << std::endl;
	// }
	// else if (op == '/')
	//{
	//	std::cout << "    movl $0, %edx" << std::endl;
	//	std::cout << "    movl %eax, %ecx" << std::endl;
	//	std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	//	std::cout << "    idiv %ecx" << std::endl;
	// }
	// else if (op == '%')
	//{
	//	std::cout << "    movl $0, %edx" << std::endl;
	//	std::cout << "    movl %eax, %ecx" << std::endl;
	//	std::cout << "    movl -" << nextIndex - 4 << "(%rbp), %eax" << std::endl;
	//	std::cout << "    idiv %ecx" << std::endl;
	//	std::cout << "    movl %edx, %eax" << std::endl;
	// }
	// nextIndex -= 4;
	// return antlrcpp::Any();
	char op = ctx->op->getText()[0];
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	if (op == '*')
	{
		cfg.current_bb->add_IRInstr(IRInstr::mul, Type::INT, {out, lhs, rhs});
	}
	else if (op == '/')
	{
		cfg.current_bb->add_IRInstr(IRInstr::div, Type::INT, {out, lhs, rhs});
	}
	else if (op == '%')
	{
		cfg.current_bb->add_IRInstr(IRInstr::mod, Type::INT, {out, lhs, rhs});
	}
	return out;
}
