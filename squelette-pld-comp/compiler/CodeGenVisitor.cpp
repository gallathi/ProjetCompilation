#include "CodeGenVisitor.h"

#include "IR.h"
#include <stack>

using namespace std;

CFG cfg;
bool debug_cfg = false;

string cond = "a";

void CodeGenVisitor::bindFunctionState(const std::string &name)
{
    currentFunction = name;
    declarationCounter = 0;
    currentParamIndex = 0;
    scopeStack.clear();

    auto it = functionStates.find(name);
    if (it == functionStates.end())
    {
        currentVarTable.clear();
        currentFrameBytes = 0;
        return;
    }

    currentVarTable = it->second.varTable;
    currentFrameBytes = it->second.compteurVar;
}

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
	cfg.gen_asm_text_section_line(cout);
    for (ifccParser::Function_defContext *fn : ctx->function_def())
    {
        visit(fn);
    }
    cfg.gen_asm_data_section(cout, dconsts);
    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitFunction_def(ifccParser::Function_defContext *ctx)
{
    bindFunctionState(ctx->VAR()->getText());
    hasReturned = false;

    cfg = CFG();
    cfg.set_function_name(currentFunction);
    cfg.current_bb = nullptr;

    for (const auto &[name, info] : currentVarTable)
    {
        cfg.add_to_symbol_table(name, info.type);
    }

    cfg.gen_asm_prologue(cout, currentFrameBytes, currentFunction == "main");

    BasicBlock *entry = new BasicBlock(&cfg, cfg.new_BB_name());
    cfg.add_bb(entry);
    cfg.current_bb = entry;
    scopeStack.push_back({});

    if (ctx->param_list() != nullptr)
    {
        for (ifccParser::ParamContext *param : ctx->param_list()->param())
        {
            visit(param);
        }
    }

    for (ifccParser::StmtContext *stmt : ctx->block()->stmt())
    {
        visit(stmt);
    }

    if (!hasReturned && ctx->type()->getText() == "int")
    {
        string zero = cfg.create_new_tempvar(Type::INT);
        cfg.add_to_symbol_table(zero, Type::INT);
        cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {zero, "0"});
        cfg.current_bb->add_IRInstr(IRInstr::return_instr, Type::INT, {zero});
        hasReturned = true;
    }

    scopeStack.pop_back();

    cfg.gen_asm(cout);
    cfg.gen_asm_epilogue(cout, currentFunction == "main");

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitParam(ifccParser::ParamContext *ctx)
{
    std::string sourceName = ctx->VAR()->getText();
    std::string scopedName = createScopedName(sourceName);
    scopeStack.back()[sourceName] = scopedName;

    cfg.current_bb->add_IRInstr(IRInstr::load_param, cfg.get_var_type(scopedName), {scopedName, std::to_string(currentParamIndex)});
    currentParamIndex++;

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    scopeStack.push_back({});

    for (auto s : ctx->stmt())
    {
        visit(s);
    }

    scopeStack.pop_back();

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlockNoAutoGen(ifccParser::BlockContext *ctx)
{
    return visitBlock(ctx);
}

antlrcpp::Any CodeGenVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx)
{
    std::string sourceName;
    if (ctx->decla_affect() != nullptr)
    {
        sourceName = ctx->decla_affect()->VAR()->getText();
    }
    else
    {
        sourceName = ctx->VAR()->getText();
    }

    std::string scopedName = createScopedName(sourceName);
    scopeStack.back()[sourceName] = scopedName;

    if (ctx->decla_affect() != nullptr)
    {
        visit(ctx->decla_affect());
    }

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

antlrcpp::Any CodeGenVisitor::visitWhile_conditional(ifccParser::While_conditionalContext *ctx)
{
	BasicBlock *condBB = new BasicBlock(&cfg, cfg.new_BB_name());
	cfg.add_bb(condBB);
	BasicBlock *bodyBB = new BasicBlock(&cfg, cfg.new_BB_name());
	cfg.add_bb(bodyBB);
	BasicBlock *nextBlock = new BasicBlock(&cfg, cfg.new_BB_name());
	cfg.add_bb(nextBlock);
	
	pair<BasicBlock*, BasicBlock*> newLoop;
	newLoop.first = condBB;
	newLoop.second = nextBlock;
	loopStack.push_back(newLoop);

	cfg.current_bb->exit_true = condBB;
	
	cfg.current_bb = condBB;
	string condVar = std::any_cast<string>(visit(ctx->expression()));

	cfg.current_bb->test_var_name = condVar;
	cfg.current_bb->exit_true = bodyBB;
	cfg.current_bb->exit_false = nextBlock;

	cfg.current_bb = bodyBB;
	visitBlockNoAutoGen(ctx->block());

	cfg.current_bb->exit_true = condBB;
	cfg.current_bb = nextBlock;
	
	loopStack.pop_back();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation_declaration(ifccParser::Affectation_declarationContext *ctx)
{
    std::string sourceName = ctx->VAR()->getText();
    std::string scopedName = createScopedName(sourceName);
    scopeStack.back()[sourceName] = scopedName;
    string value = std::any_cast<string>(visit(ctx->expression()));
    
    if (cfg.get_var_type(value) == Type::INT && cfg.get_var_type(scopedName) == Type::DOUBLE) {
    	string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::int_to_double, cfg.get_var_type(value), {conversion, value});
		value = conversion;
    } else if (cfg.get_var_type(value) == Type::DOUBLE && cfg.get_var_type(scopedName) == Type::INT) {
    	string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::double_to_int, cfg.get_var_type(value), {conversion, value});
		value = conversion;
    }
    cfg.current_bb->add_IRInstr(IRInstr::copy, cfg.get_var_type(scopedName), {scopedName, value});

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	std::string op = ctx->op->getText();
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	string value = std::any_cast<string>(visit(ctx->expression()));
	
	if (cfg.get_var_type(value) == Type::INT && cfg.get_var_type(varName) == Type::DOUBLE) {
    	string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::int_to_double, cfg.get_var_type(value), {conversion, value});
		value = conversion;
    } else if (cfg.get_var_type(value) == Type::DOUBLE && cfg.get_var_type(varName) == Type::INT) {
    	string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::double_to_int, cfg.get_var_type(value), {conversion, value});
		value = conversion;
    }

	if (op == "=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::copy, cfg.get_var_type(varName), {varName, value});
	}
	else if (op == "-=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::sub, cfg.get_var_type(varName), {varName, varName, value});
	}
	else if (op == "+=")
	{
		cfg.current_bb->add_IRInstr(IRInstr::add, cfg.get_var_type(varName), {varName, varName, value});
	}

	return varName;
}
antlrcpp::Any CodeGenVisitor::visitDecla_affect(ifccParser::Decla_affectContext *ctx)
{
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	string value = std::any_cast<string>(visit(ctx->expression()));

	cfg.current_bb->add_IRInstr(IRInstr::copy, cfg.get_var_type(varName), {varName, value});

	return varName;
}
antlrcpp::Any CodeGenVisitor::visitPre_incr(ifccParser::Pre_incrContext *ctx)
{
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	string tempVar = cfg.create_new_tempvar(cfg.get_var_type(varName));
	cfg.add_to_symbol_table(tempVar, cfg.get_var_type(varName));
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, cfg.get_var_type(varName), {tempVar, "1"});
	if (cfg.get_var_type(varName) == Type::DOUBLE) {
		string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::int_to_double, Type::DOUBLE, {conversion, tempVar});
		tempVar = conversion;
	}
	cfg.current_bb->add_IRInstr(IRInstr::add, cfg.get_var_type(varName), {varName, varName, tempVar});

	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPre_decr(ifccParser::Pre_decrContext *ctx)
{
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	string tempVar = cfg.create_new_tempvar(cfg.get_var_type(varName));
	cfg.add_to_symbol_table(tempVar, cfg.get_var_type(varName));
	cfg.current_bb->add_IRInstr(IRInstr::ldconst, cfg.get_var_type(varName), {tempVar, "1"});
	if (cfg.get_var_type(varName) == Type::DOUBLE) {
		string conversion = cfg.create_new_tempvar(Type::INT);
		cfg.add_to_symbol_table(conversion, Type::INT);
		cfg.current_bb->add_IRInstr(IRInstr::int_to_double, Type::DOUBLE, {conversion, tempVar});
		tempVar = conversion;
	}
	cfg.current_bb->add_IRInstr(IRInstr::sub, cfg.get_var_type(varName), {varName, varName, tempVar});

	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPost_incr(ifccParser::Post_incrContext *ctx)
{
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	postfixOps[varName] = "++";

	return varName;
}

antlrcpp::Any CodeGenVisitor::visitPost_decr(ifccParser::Post_decrContext *ctx)
{
	std::string varSource = ctx->VAR()->getText();
	std::string varName = resolveVisibleVar(varSource);
	postfixOps[varName] = "--";

	return varName;
}



antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    if (ctx->expression() == nullptr)
    {
        string zero = cfg.create_new_tempvar(Type::INT);
        cfg.add_to_symbol_table(zero, Type::INT);
        cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::INT, {zero, "0"});
        cfg.current_bb->add_IRInstr(IRInstr::return_instr, Type::INT, {zero});
    }
    else
    {
        string value = std::any_cast<string>(visit(ctx->expression()));
        if (cfg.get_var_type(value) == Type::DOUBLE) {
        	string conversion = cfg.create_new_tempvar(Type::INT);
   			cfg.add_to_symbol_table(conversion, Type::INT);
   			cfg.current_bb->add_IRInstr(IRInstr::double_to_int, cfg.get_var_type(value), {conversion, value});
   			value = conversion;
   		}
        cfg.current_bb->add_IRInstr(IRInstr::return_instr, cfg.get_var_type(value), {value});
    }
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

antlrcpp::Any CodeGenVisitor::visitDconst(ifccParser::DconstContext *ctx)
{
    string tempVar = cfg.create_new_tempvar(Type::DOUBLE);
    cfg.add_to_symbol_table(tempVar, Type::DOUBLE);
    std::string newName = ".LC" + std::to_string(nextDConstIndex);
    dconsts[newName] = stod(ctx->DCONST()->getText());
    cfg.current_bb->add_IRInstr(IRInstr::ldconst, Type::DOUBLE, {tempVar, newName});
    nextDConstIndex++;

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
    if (cfg.get_var_type(rhs) == Type::DOUBLE) {
    	string conversion = cfg.create_new_tempvar(Type::INT);
    	cfg.add_to_symbol_table(conversion, Type::INT);
    	cfg.current_bb->add_IRInstr(IRInstr::double_to_int, Type::INT, {conversion, rhs});
    }
    string out = cfg.create_new_tempvar(cfg.get_var_type(rhs));
    cfg.add_to_symbol_table(out, cfg.get_var_type(rhs));
    cfg.current_bb->add_IRInstr(IRInstr::neg, cfg.get_var_type(rhs), {out, rhs});
    return out;
}

antlrcpp::Any CodeGenVisitor::visitNot(ifccParser::NotContext *ctx)
{
    string rhs = std::any_cast<string>(visit(ctx->expression()));
    string out = cfg.create_new_tempvar(cfg.get_var_type(rhs));
    cfg.add_to_symbol_table(out, cfg.get_var_type(rhs));
    cfg.current_bb->add_IRInstr(IRInstr::bool_not, cfg.get_var_type(rhs), {out, rhs});
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
    
    Type outType = cfg.get_var_type(lhs) == Type::DOUBLE || cfg.get_var_type(rhs) == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    
    if (outType == Type::DOUBLE && (cfg.get_var_type(lhs) == Type::INT || cfg.get_var_type(rhs) == Type::INT)) {
   		string conversion = cfg.create_new_tempvar(outType);
   		cfg.add_to_symbol_table(conversion, outType);
   		if (cfg.get_var_type(lhs) == Type::INT) {
   			cfg.current_bb->add_IRInstr(IRInstr::int_to_double, outType, {conversion, lhs});
   			lhs = conversion;
   		} else {
   			cfg.current_bb->add_IRInstr(IRInstr::int_to_double, outType, {conversion, rhs});
   			rhs = conversion;
   		}
    }
    
    string out = cfg.create_new_tempvar(outType);
    cfg.add_to_symbol_table(out, outType);
    
    if (op == '+')
    {
        cfg.current_bb->add_IRInstr(IRInstr::add, outType, {out, lhs, rhs});
    }
    else if (op == '-')
    {
        cfg.current_bb->add_IRInstr(IRInstr::sub, outType, {out, lhs, rhs});
    }
    return out;
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    char op = ctx->op->getText()[0];
    string lhs = std::any_cast<string>(visit(ctx->expression(0)));
    string rhs = std::any_cast<string>(visit(ctx->expression(1)));
    Type outType = cfg.get_var_type(lhs) == Type::DOUBLE || cfg.get_var_type(rhs) == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    
    if (outType == Type::DOUBLE && (cfg.get_var_type(lhs) == Type::INT || cfg.get_var_type(rhs) == Type::INT)) {
   		string conversion = cfg.create_new_tempvar(outType);
   		cfg.add_to_symbol_table(conversion, outType);
   		if (cfg.get_var_type(lhs) == Type::INT) {
   			cfg.current_bb->add_IRInstr(IRInstr::int_to_double, outType, {conversion, lhs});
   			lhs = conversion; 
   		} else {
   			cfg.current_bb->add_IRInstr(IRInstr::int_to_double, outType, {conversion, rhs});
   			rhs = conversion;
   		}
    }
    
    string out = cfg.create_new_tempvar(outType);
    cfg.add_to_symbol_table(out, outType);
    if (op == '*')
    {
        cfg.current_bb->add_IRInstr(IRInstr::mul, outType, {out, lhs, rhs});
    }
    else if (op == '/')
    {
        cfg.current_bb->add_IRInstr(IRInstr::div, outType, {out, lhs, rhs});
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
    Type inType = cfg.get_var_type(lhs) == Type::DOUBLE || cfg.get_var_type(rhs) == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    string out = cfg.create_new_tempvar(Type::INT);
    cfg.add_to_symbol_table(out, Type::INT);
    if (op == "<=")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_elt, inType, {out, lhs, rhs});
    }
    else if (op == "<")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_lt, inType, {out, lhs, rhs});
    }
    else if (op == ">=")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_egt, inType, {out, lhs, rhs});
    }
    else if (op == ">")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_gt, inType, {out, lhs, rhs});
    }
    return out;
}

antlrcpp::Any CodeGenVisitor::visitEq(ifccParser::EqContext *ctx)
{
    string op = ctx->op->getText();
    string lhs = std::any_cast<string>(visit(ctx->expression(0)));
    string rhs = std::any_cast<string>(visit(ctx->expression(1)));
    Type inType = cfg.get_var_type(lhs) == Type::DOUBLE || cfg.get_var_type(rhs) == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    string out = cfg.create_new_tempvar(Type::INT);
    cfg.add_to_symbol_table(out, Type::INT);
    if (op == "==")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_eq, inType, {out, lhs, rhs});
    }
    else if (op == "!=")
    {
        cfg.current_bb->add_IRInstr(IRInstr::cmp_neq, inType, {out, lhs, rhs});
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
    (void)ctx;
    string out = cfg.create_new_tempvar(Type::INT);
    cfg.add_to_symbol_table(out, Type::INT);
    cfg.current_bb->add_IRInstr(IRInstr::getchar, Type::INT, {out});
    return out;
}

antlrcpp::Any CodeGenVisitor::visitCall(ifccParser::CallContext *ctx)
{
    string out = cfg.create_new_tempvar(Type::INT);
    cfg.add_to_symbol_table(out, Type::INT);

    vector<string> params;
    params.push_back(ctx->VAR()->getText());
    params.push_back(out);
    if (ctx->arg_list() != nullptr)
    {
        for (ifccParser::ExpressionContext *arg : ctx->arg_list()->expression())
        {
            params.push_back(std::any_cast<string>(visit(arg)));
        }
    }
    cfg.current_bb->add_IRInstr(IRInstr::call, Type::INT, params);

    auto sigIt = functionSignatures.find(ctx->VAR()->getText());
    if (sigIt != functionSignatures.end() && sigIt->second.returnType == Type::VOID)
    {
        return string("");
    }

    return out;
}

antlrcpp::Any CodeGenVisitor::visitStmt(ifccParser::StmtContext *ctx)
{
	if (ctx->CONTINUE() != nullptr) {
		cfg.current_bb->add_IRInstr(IRInstr::jump, Type::INT, {loopStack.back().first->label});
	} else if (ctx->BREAK() != nullptr) {
		cfg.current_bb->add_IRInstr(IRInstr::jump, Type::INT, {loopStack.back().second->label});
	}
	visitChildren(ctx);
	for (auto &op : postfixOps)
	{
		Type type = cfg.get_var_type(op.first);
		string tempVar = cfg.create_new_tempvar(type);
		cfg.add_to_symbol_table(tempVar, type);

		cfg.current_bb->add_IRInstr(IRInstr::ldconst, type, {tempVar, "1"});
		if (cfg.get_var_type(op.first) == Type::DOUBLE) {
			string conversion = cfg.create_new_tempvar(Type::INT);
			cfg.add_to_symbol_table(conversion, Type::INT);
			cfg.current_bb->add_IRInstr(IRInstr::int_to_double, Type::DOUBLE, {conversion, tempVar});
			tempVar = conversion;
		}
		if (op.second == "++")
		{
			cfg.current_bb->add_IRInstr(IRInstr::add, type, {op.first, op.first, tempVar});
		}
		else if (op.second == "--")
		{
			cfg.current_bb->add_IRInstr(IRInstr::sub, type, {op.first, op.first, tempVar});
		}
	}
	postfixOps.clear();
	return antlrcpp::Any();
}
