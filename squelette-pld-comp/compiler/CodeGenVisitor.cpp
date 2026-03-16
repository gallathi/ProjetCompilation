#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"
#include <stack>

using namespace std;

CFG cfg;
bool debug_cfg = true;

string cond = "a";

std::string CodeGenVisitor::createScopedName(const std::string &name)
{
	declarationCounter++;
	return name + "#" + std::to_string(declarationCounter);
}

std::string CodeGenVisitor::resolveVisibleVar(const std::string &name) const
{
	for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it)
	{
		auto found = it->find(name);
		if (found != it->end())
		{
			return found->second;
		}
	}
	return "";
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
	hasReturned = false;
	declarationCounter = 0;
	scopeStack.clear();
	cfg.current_bb = nullptr;

	for (const auto &[name, info] : varTable)
	{
		cfg.add_to_symbol_table(name, Type::INT);
	}

	// cfg.gen_asm_prologue(cout, compteurVar);
	visit(ctx->block());
	// cfg.gen_asm(cout);
	// cfg.gen_asm_epilogue(cout);

	if (debug_cfg)
		cout << cfg;
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
	BasicBlock *bb = new BasicBlock(&cfg, cfg.new_BB_name());
	if (cfg.current_bb != nullptr && !hasReturned)
	{
		cfg.current_bb->exit_true = bb;
	}
	cfg.add_bb(bb);
	cfg.current_bb = bb;

	scopeStack.push_back({});

	for (auto s : ctx->stmt())
	{
		if (hasReturned)
		{
			break;
		}
		visit(s);
	}

	scopeStack.pop_back();

	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlockNoAutoGen(ifccParser::BlockContext *ctx)
{
	scopeStack.push_back({});

	for (auto s : ctx->stmt())
	{
		if (hasReturned)
		{
			break;
		}
		visit(s);
	}

	scopeStack.pop_back();

	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx)
{
	std::string sourceName = ctx->VAR()->getText();
	std::string scopedName = createScopedName(sourceName);
	scopeStack.back()[sourceName] = scopedName;

	if (ctx->declaration_var() != nullptr)
	{
		visit(ctx->declaration_var());
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
	visitBlockNoAutoGen(ctx->block());
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
	visitBlockNoAutoGen(ctx->block());
	cfg.current_bb->exit_true = endifStack.top();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitConditional(ifccParser::ConditionalContext *ctx)
{
	string condVar = std::any_cast<string>(visit(ctx->expression()));

	BasicBlock *endifBB = new BasicBlock(&cfg, cfg.new_BB_name());
	endifStack.push(endifBB);

	cfg.current_bb->test_var_name = condVar;

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
	visitBlockNoAutoGen(ctx->block());

	// n'importe quoi qui vient aprés le if, il doit pointer vers le endif final
	cfg.current_bb->exit_true = endifStack.top(); // jump inconditionelle

	cfg.add_bb(endifStack.top());
	cfg.current_bb = endifStack.top(); // the rest of the program will add to the endif block

	endifStack.pop();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	std::string sourceName = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(sourceName);
	string value = std::any_cast<string>(visit(ctx->expression()));
	cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {varName, value});

	return varName;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
	string value = std::any_cast<string>(visit(ctx->expression()));
	cfg.current_bb->add_IRInstr(IRInstr::return_instr, Type::INT, {value});
	hasReturned = true;
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
	string sourceName = ctx->VAR()->getText();
	string varName = resolveVisibleVar(sourceName);
	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPar(ifccParser::ParContext *ctx)
{
	return visit(ctx->expression());
}

antlrcpp::Any CodeGenVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
{
	string rhs = std::any_cast<string>(visit(ctx->expression()));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::neg, Type::INT, {out, rhs});
	return out;
}

antlrcpp::Any CodeGenVisitor::visitNot(ifccParser::NotContext *ctx)
{
	string rhs = std::any_cast<string>(visit(ctx->expression()));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::bool_not, Type::INT, {out, rhs});
	return out;
}

antlrcpp::Any CodeGenVisitor::visitBitwise_and(ifccParser::Bitwise_andContext *ctx)
{
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::bitwise_and, Type::INT, {out, lhs, rhs});
	return out;
}

antlrcpp::Any CodeGenVisitor::visitBitwise_xor(ifccParser::Bitwise_xorContext *ctx)
{
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::bitwise_xor, Type::INT, {out, lhs, rhs});
	return out;
}

antlrcpp::Any CodeGenVisitor::visitBitwise_or(ifccParser::Bitwise_orContext *ctx)
{
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::bitwise_or, Type::INT, {out, lhs, rhs});
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

antlrcpp::Any CodeGenVisitor::visitComp(ifccParser::CompContext *ctx)
{
	string op = ctx->op->getText();
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	if (op == "<=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_elt, Type::INT, {out, lhs, rhs});
	}
	else if (op == "<")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_lt, Type::INT, {out, lhs, rhs});
	}
	else if (op == ">=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_egt, Type::INT, {out, lhs, rhs});
	}
	else if (op == ">")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_gt, Type::INT, {out, lhs, rhs});
	}
	return out;
}

antlrcpp::Any CodeGenVisitor::visitEq(ifccParser::EqContext *ctx)
{
	string op = ctx->op->getText();
	string lhs = std::any_cast<string>(visit(ctx->expression(0)));
	string rhs = std::any_cast<string>(visit(ctx->expression(1)));
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	if (op == "==")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_eq, Type::INT, {out, lhs, rhs});
	}
	else if (op == "!=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::cmp_neq, Type::INT, {out, lhs, rhs});
	}
	return out;
}

static int unescapeChar(const std::string &s)
{
	std::string content = s.substr(1, s.size() - 2);
	int result = 0;

	for (size_t i = 0; i < content.size(); ++i)
	{
		int currentChar = 0;
		if (content[i] == '\\' && i + 1 < content.size())
		{
			i++;
			switch (content[i])
			{
			case 'n':
				currentChar = '\n';
				break;
			case 't':
				currentChar = '\t';
				break;
			case 'r':
				currentChar = '\r';
				break;
			case '\\':
				currentChar = '\\';
				break;
			case '\'':
				currentChar = '\'';
				break;
			case '0':
				currentChar = '\0';
				break;
			default:
				currentChar = content[i];
				break;
			}
		}
		else
		{
			currentChar = (unsigned char)content[i];
		}

		result = (result << 8) | currentChar;
	}
	return result;
}

antlrcpp::Any CodeGenVisitor::visitCharconst(ifccParser::CharconstContext *ctx)
{
	string charLiteral = ctx->CHARCONST()->getText();
	int charValue = unescapeChar(charLiteral);
	string tempVar = cfg.create_new_tempvar(Type::CHAR);
	cfg.add_to_symbol_table(tempVar, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, to_string(charValue)});

	return tempVar;
}

antlrcpp::Any CodeGenVisitor::visitPutchar(ifccParser::PutcharContext *ctx)
{
	string arg = std::any_cast<string>(visit(ctx->expression()));
	cfg.current_bb->add_IRInstr(IRInstr::putchar, Type::INT, {arg});
	return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitGetchar(ifccParser::GetcharContext *ctx)
{
	string out = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(out, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::getchar, Type::INT, {out});
	return out;
}
