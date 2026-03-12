#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"

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

	cfg.gen_asm_prologue(cout);
	visit(ctx->block());
	cfg.gen_asm(cout);
	cfg.gen_asm_epilogue(cout);

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

antlrcpp::Any CodeGenVisitor::visitConditional(ifccParser::ConditionalContext *ctx)
{

	cfg.current_bb->test_var_name = cond;
	cond[0]++;
	BasicBlock *thenBB = new BasicBlock(&cfg, cfg.new_BB_name());
	BasicBlock *elseBB = nullptr;

	bool elseConditional = false;
	bool elseBlock = false;

	/// checking if there is an else, and if its a block or another conditional
	if (ctx->block().size() == 2)
	{
		elseBB = new BasicBlock(&cfg, cfg.new_BB_name());
		elseBlock = true;
	}
	if (ctx->conditional() != nullptr)
	{
		elseBB = new BasicBlock(&cfg, cfg.new_BB_name());
		elseConditional = true;
	}
	BasicBlock *endifBB = new BasicBlock(&cfg, cfg.new_BB_name());

	// ading the thern bb and endif to it
	cfg.add_bb(thenBB);
	thenBB->exit_true = endifBB;

	// checking if there is an else block or conditional, and adding the proper exits to the current CFG
	if (elseBB != nullptr)
	{
		cfg.add_bb(elseBB);
		elseBB->exit_true = endifBB;
		cfg.current_bb->exit_false = elseBB;
	}
	else
	{
		cfg.current_bb->exit_false = endifBB;
	}
	cfg.add_bb(endifBB);
	cfg.current_bb->exit_true = thenBB;

	cfg.current_bb = thenBB;
	visit(ctx->block(0));

	if (elseBB != nullptr)
	{
		cfg.current_bb = elseBB;
		if (elseConditional)
			visit(ctx->conditional());
		else if (elseBlock)
		{
			visit(ctx->block(1));
		}
	}

	cfg.current_bb = endifBB;

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
