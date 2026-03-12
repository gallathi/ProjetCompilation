#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"

using namespace std;

CFG cfg;

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
	for (const auto &[name, info] : varTable)
	{
		cfg.add_to_symbol_table(name, Type::INT);
	}

	cfg.gen_asm_prologue(cout, compteurVar);
	visit(ctx->block());
	cfg.gen_asm(cout);
	cfg.gen_asm_epilogue(cout);

	visit(ctx->block());
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

	return antlrcpp::Any();
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

antlrcpp::Any CodeGenVisitor::visitPar(ifccParser::ParContext *ctx)
{
	return visit(ctx->expression());
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
