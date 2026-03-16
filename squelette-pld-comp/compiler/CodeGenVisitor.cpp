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
	std::string op = ctx->op->getText();
	std::string varName = ctx->VAR()->getText();
	string value = std::any_cast<string>(visit(ctx->expression()));
	
	if (op == "=") {
		cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {varName, value});
	} else if (op == "-=") {
		cfg.current_bb->add_IRInstr(IRInstr::sub, Type::INT, {varName, varName, value});
	} else if (op == "+=") {
		cfg.current_bb->add_IRInstr(IRInstr::add, Type::INT, {varName, varName, value});
	}

	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPre_incr(ifccParser::Pre_incrContext *ctx)
{
	string tempVar = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(tempVar, Type::INT);
	string varName = ctx->VAR()->getText();
	
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, "1"});
	cfg.current_bb->add_IRInstr(IRInstr::add, Type::INT, {varName, varName, tempVar});
	
	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPre_decr(ifccParser::Pre_decrContext *ctx)
{
	string tempVar = cfg.create_new_tempvar(Type::INT);
	cfg.add_to_symbol_table(tempVar, Type::INT);
	string varName = ctx->VAR()->getText();
	
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, "1"});
	cfg.current_bb->add_IRInstr(IRInstr::sub, Type::INT, {varName, varName, tempVar});
	
	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPost_incr(ifccParser::Post_incrContext *ctx)
{
	string varName = ctx->VAR()->getText();
	postfixOps[varName] = "++";
	
	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPost_decr(ifccParser::Post_decrContext *ctx)
{
	string varName = ctx->VAR()->getText();
	postfixOps[varName] = "--";
	
	return varName;
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

antlrcpp::Any CodeGenVisitor::visitNot(ifccParser::NotContext *ctx)
{
	string rhs = std::any_cast<string>(visit(ctx->expression())); // operand result
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

static int unescapeChar(const std::string &s) {
    // Remove the surrounding single quotes
    // 'a' -> a, '\n' -> \n, 'abc' -> abc
    std::string content = s.substr(1, s.size() - 2);
    
    int result = 0;

    for (size_t i = 0; i < content.size(); ++i) {
        int currentChar = 0;
        if (content[i] == '\\' && i + 1 < content.size()) {
            i++; // Move to the escaped character
            switch (content[i]) {
                case 'n':  currentChar = '\n'; break;
                case 't':  currentChar = '\t'; break;
                case 'r':  currentChar = '\r'; break;
                case '\\': currentChar = '\\'; break;
                case '\'': currentChar = '\''; break;
                case '0':  currentChar = '\0'; break;
                default:   currentChar = content[i]; break;
            }
        } else {
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

antlrcpp::Any CodeGenVisitor::visitStmt(ifccParser::StmtContext *ctx)
{
    visitChildren(ctx);
    for (auto& op : postfixOps) {
    	string tempVar = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(tempVar, Type::INT);
		
		cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, "1"});
		if (op.second == "++") {
			cfg.current_bb->add_IRInstr(IRInstr::add, Type::INT, {op.first, op.first, tempVar});
		} else if (op.second == "--") {
			cfg.current_bb->add_IRInstr(IRInstr::sub, Type::INT, {op.first, op.first, tempVar});
		}
    }
    postfixOps.clear();
	return antlrcpp::Any();
}
