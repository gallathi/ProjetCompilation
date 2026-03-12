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

\        result = (result << 8) | currentChar;
    }
    return result;
}

antlrcpp::Any CodeGenVisitor::visitCharconst(ifccParser::CharconstContext *ctx)
{
	string charLiteral = ctx->CHAR()->getText();
	int charValue = unescapeChar(charLiteral);
	string tempVar = cfg.create_new_tempvar(Type::CHAR);
	cfg.add_to_symbol_table(tempVar, Type::INT);
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {tempVar, to_string(charValue)});

	return tempVar;
}
