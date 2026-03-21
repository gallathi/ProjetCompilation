#include "CodeGenVisitor.h"

#include "IR.h"
#include "type.h"

using namespace std;

CFG cfg;

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
    for (ifccParser::Function_defContext *fn : ctx->function_def())
    {
        visit(fn);
    }
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
        cfg.add_to_symbol_table(name, Type::INT);
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
        if (hasReturned)
        {
            break;
        }
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

    cfg.current_bb->add_IRInstr(IRInstr::load_param, Type::INT, {scopedName, std::to_string(currentParamIndex)});
    currentParamIndex++;

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
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

antlrcpp::Any CodeGenVisitor::visitBlockNoAutoGen(ifccParser::BlockContext *ctx)
{
    return visitBlock(ctx);
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

antlrcpp::Any CodeGenVisitor::visitAffectation_declaration(ifccParser::Affectation_declarationContext *ctx)
{
    std::string sourceName = ctx->VAR()->getText();
    std::string scopedName = createScopedName(sourceName);
    scopeStack.back()[sourceName] = scopedName;

    string value = std::any_cast<string>(visit(ctx->expression()));
    cfg.current_bb->add_IRInstr(IRInstr::copy, Type::INT, {scopedName, value});

    return antlrcpp::Any();
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
        cfg.current_bb->add_IRInstr(IRInstr::return_instr, Type::INT, {value});
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
