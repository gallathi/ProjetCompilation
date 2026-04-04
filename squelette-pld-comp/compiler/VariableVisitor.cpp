#include "VariableVisitor.h"

#include <iostream>
#include <stack>
#include <utility>
#include <vector>

TypeSizes typeSizes;

void VariableVisitor::clearScopedVars()
{
    while (!scopedVars.empty())
    {
        scopedVars.pop();
    }
}

bool VariableVisitor::checkIfVarInCurrentBlock(const std::string &var) const
{
    std::stack<ScopedVarInfo> temp = scopedVars;
    while (!temp.empty())
    {
        auto top = temp.top();
        if (top.block < scopeBlock)
        {
            break;
        }
        if (top.block == scopeBlock && top.name == var)
        {
            return true;
        }
        temp.pop();
    }
    return false;
}

std::string VariableVisitor::resolveVisibleVarSymbol(const std::string &var) const
{
    std::stack<ScopedVarInfo> temp = scopedVars;
    while (!temp.empty())
    {
        auto top = temp.top();
        if (top.name == var)
        {
            return top.symbol;
        }
        temp.pop();
    }
    return "";
}

int VariableVisitor::getErrorCount()
{
    return errorCount;
}

int VariableVisitor::getNextOffset()
{
    auto it = functionStates.find("main");
    if (it == functionStates.end())
    {
        return 0;
    }
    return it->second.nextIndex;
}

int VariableVisitor::getCompteurVar()
{
    auto it = functionStates.find("main");
    if (it == functionStates.end())
    {
        return 0;
    }
    return it->second.compteurVar;
}

std::map<std::string, varInfo> VariableVisitor::getVarTable()
{
    auto it = functionStates.find("main");
    if (it == functionStates.end())
    {
        return {};
    }
    return it->second.varTable;
}

const std::map<std::string, FunctionSignature> &VariableVisitor::getFunctionSignatures() const
{
    return functionSignatures;
}

const std::map<std::string, FunctionSemanticState> &VariableVisitor::getFunctionStates() const
{
    return functionStates;
}

FunctionSemanticState &VariableVisitor::currentState()
{
    return functionStates[currentFunction];
}

void VariableVisitor::reportError(const std::string &msg)
{
    if (debug)
    {
        std::cout << "ERREUR : " << msg << std::endl;
    }
    errorCount++;
}

Type VariableVisitor::parseTypeText(const std::string &text) const
{
    if (text == "void")
    {
        return Type::VOID;
    } else if (text == "char")
    {
    	return Type::CHAR;
    } else if (text == "double")
    {
    	return Type::DOUBLE;
    } 
    return Type::INT;
}

Type VariableVisitor::evalExpr(ifccParser::ExpressionContext *ctx)
{
    return std::any_cast<Type>(visit(ctx));
}

std::string VariableVisitor::declareVar(const std::string &varName, bool affected)
{
    if (checkIfVarInCurrentBlock(varName))
    {
        reportError("La variable " + varName + " est declaree plusieurs fois.");
        return "";
    }

    FunctionSemanticState &state = currentState();
    state.declarationCounter++;
    std::string symbolName = varName + "#" + std::to_string(state.declarationCounter);

    varInfo info;
    state.nextIndex += typeSizes.getTypeSize(currentType);
    info.index = state.nextIndex;
    state.compteurVar += state.nextIndex;
    info.used = false;
    info.affected = affected;
    info.type = currentType;
    state.varTable[symbolName] = info;

    scopedVars.push({varName, symbolName, scopeBlock});

    if (debug)
    {
        std::cout << "declaration de " << varName << " (" << symbolName << ") : " << info.index << std::endl;
    }

    return symbolName;
}

void VariableVisitor::allocateTemporary(Type t)
{
    if (currentFunction.empty())
    {
        return;
    }
    currentState().compteurVar += typeSizes.getTypeSize(t);
}

std::any VariableVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    errorCount = 0;
    functionSignatures.clear();
    functionStates.clear();
    currentFunction.clear();
    clearScopedVars();
    scopeBlock = 0;

    for (ifccParser::Function_defContext *fnCtx : ctx->function_def())
    {
        std::string fnName = fnCtx->VAR()->getText();
        if (functionSignatures.count(fnName) != 0)
        {
            reportError("La fonction " + fnName + " est definie plusieurs fois.");
            continue;
        }

        FunctionSignature sig;
        sig.returnType = parseTypeText(fnCtx->type()->getText());

        if (fnCtx->param_list() != nullptr)
        {
            for (ifccParser::ParamContext *paramCtx : fnCtx->param_list()->param())
            {
                Type pType = parseTypeText(paramCtx->type()->getText());
                if (pType == Type::VOID)
                {
                    reportError("Le parametre " + paramCtx->VAR()->getText() + " ne peut pas etre void.");
                }
                sig.paramTypes.push_back(pType);
            }
        }

        functionSignatures[fnName] = sig;
    }

    auto mainIt = functionSignatures.find("main");
    if (mainIt == functionSignatures.end())
    {
        reportError("Le programme doit definir main.");
    }
    else
    {
        if (mainIt->second.returnType != Type::INT)
        {
            reportError("main doit retourner int.");
        }
        if (!mainIt->second.paramTypes.empty())
        {
            reportError("main ne doit pas avoir de parametres.");
        }
    }

    // Second pass: bodies, now that all signatures are known. This enables recursion.
    for (ifccParser::Function_defContext *fnCtx : ctx->function_def())
    {
        visit(fnCtx);
    }

    return {};
}

std::any VariableVisitor::visitFunction_def(ifccParser::Function_defContext *ctx)
{
    currentFunction = ctx->VAR()->getText();
    auto sigIt = functionSignatures.find(currentFunction);
    if (sigIt == functionSignatures.end())
    {
        return {};
    }

    currentReturnType = sigIt->second.returnType;
    functionStates[currentFunction] = FunctionSemanticState();
    clearScopedVars();
    scopeBlock = 1;
    hasReturn = false;

    if (ctx->param_list() != nullptr)
    {
        size_t i = 0;
        for (ifccParser::ParamContext *paramCtx : ctx->param_list()->param())
        {
            Type pType = sigIt->second.paramTypes[i++];
            if (pType == Type::VOID)
            {
                continue;
            }
            std::string symbol = declareVar(paramCtx->VAR()->getText(), true);
            if (!symbol.empty())
            {
                currentState().varTable[symbol].used = true;
            }
        }
    }

    for (ifccParser::StmtContext *stmtCtx : ctx->block()->stmt())
    {
        visit(stmtCtx);
    }

    while (!scopedVars.empty() && scopedVars.top().block == scopeBlock)
    {
        scopedVars.pop();
    }

    if (currentReturnType == Type::INT && !hasReturn && currentFunction != "main")
    {
        reportError("La fonction " + currentFunction + " doit retourner une valeur.");
    }

    bool hasUnused = false;
    for (const auto &entry : currentState().varTable)
    {
        if (!entry.second.used)
        {
            hasUnused = true;
            break;
        }
    }
    if (hasUnused)
    {
        reportError("Toutes les variables declarees dans " + currentFunction + " n'ont pas ete utilisees.");
    }

    currentFunction.clear();
    scopeBlock = 0;
    clearScopedVars();
    return {};
}

std::any VariableVisitor::visitParam_list(ifccParser::Param_listContext *ctx)
{
    return visitChildren(ctx);
}

std::any VariableVisitor::visitParam(ifccParser::ParamContext *ctx)
{
    return parseTypeText(ctx->type()->getText());
}

std::any VariableVisitor::visitType(ifccParser::TypeContext *ctx)
{
    return parseTypeText(ctx->getText());
}

std::any VariableVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    scopeBlock++;

    for (ifccParser::StmtContext *stmtCtx : ctx->stmt())
    {
        visit(stmtCtx);
    }

    while (!scopedVars.empty() && scopedVars.top().block == scopeBlock)
    {
        scopedVars.pop();
    }

    scopeBlock--;
    return {};
}

std::any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
{
    currentType = parseTypeText(ctx->type_decla()->getText());
    visit(ctx->declaration_var());
    return {};
}

std::any VariableVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx)
{
    if (ctx->decla_affect() != nullptr)
    {
        declareVar(ctx->decla_affect()->VAR()->getText(), false);
        visit(ctx->decla_affect());
    }
    else
    {
        declareVar(ctx->VAR()->getText(), false);
    }
    if (ctx->declaration_var() != nullptr)
    {
        visit(ctx->declaration_var());
    }
    return {};
}

std::any VariableVisitor::visitAffectation_declaration(ifccParser::Affectation_declarationContext *ctx)
{
    std::string symbol = declareVar(ctx->VAR()->getText(), false);
    Type rhsType = evalExpr(ctx->expression());
    if (rhsType == Type::VOID)
    {
        reportError("Initialisation invalide de " + ctx->VAR()->getText() + " avec une expression void.");
    }
    if (!symbol.empty())
    {
        currentState().varTable[symbol].affected = true;
    }
    if (rhsType == Type::INT && currentState().varTable[symbol].type == Type::DOUBLE) {
    	allocateTemporary(Type::DOUBLE);
    } else if (rhsType == Type::DOUBLE && currentState().varTable[symbol].type == Type::INT) {
    	allocateTemporary(Type::INT);
    }
    return {};
}

std::any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    Type rhsType = evalExpr(ctx->expression());
    if (rhsType == Type::VOID)
    {
        reportError("Affectation invalide d'une expression void a " + ctx->VAR()->getText() + ".");
    }

    std::string var = ctx->VAR()->getText();
    std::string symbol = resolveVisibleVarSymbol(var);
    if (symbol.empty())
    {
        reportError("La variable " + var + " est utilisee avant declaration.");
        return Type::INT;
    }

    currentState().varTable[symbol].used = true;
    currentState().varTable[symbol].affected = true;

    if (debug)
    {
        std::cout << "affectation de la variable " << var << " (" << symbol << ")" << std::endl;
    }
    if (rhsType == Type::INT && currentState().varTable[symbol].type == Type::DOUBLE) {
    	allocateTemporary(Type::DOUBLE);
    } else if (rhsType == Type::DOUBLE && currentState().varTable[symbol].type == Type::INT) {
    	allocateTemporary(Type::INT);
    }
    return rhsType;
}
antlrcpp::Any VariableVisitor::visitDecla_affect(ifccParser::Decla_affectContext *ctx)
{
	visit(ctx->expression());
	std::string var = ctx->VAR()->getText();
	std::string symbolName = resolveVisibleVarSymbol(var);

	if (symbolName.empty())
	{
		if (debug)
			reportError("La variable " + symbolName + " est utilisée avant déclaration.");
		return 0;
	}

	//varTable[symbolName].used = true;
	//varTable[symbolName].affected = true;
    currentState().varTable[symbolName].used = true;
    currentState().varTable[symbolName].affected = true;


	if (debug)
		std::cout << "affectation de la variable " << var << std::endl;
	return 0;
}

antlrcpp::Any VariableVisitor::visitPre_incr(ifccParser::Pre_incrContext *ctx)
{
	
	Type t = currentState().varTable[resolveVisibleVarSymbol(ctx->VAR()->getText())].type;
	if (t == Type::DOUBLE) {
		allocateTemporary(t);
	}
    allocateTemporary(t);
	return t;
}

antlrcpp::Any VariableVisitor::visitPre_decr(ifccParser::Pre_decrContext *ctx)
{
	
	Type t = currentState().varTable[resolveVisibleVarSymbol(ctx->VAR()->getText())].type;
	if (t == Type::DOUBLE) {
		allocateTemporary(t);
	}
    allocateTemporary(t);
	return t;
}

antlrcpp::Any VariableVisitor::visitPost_incr(ifccParser::Post_incrContext *ctx)
{
	Type t = currentState().varTable[resolveVisibleVarSymbol(ctx->VAR()->getText())].type;
	if (t == Type::DOUBLE) {
		allocateTemporary(t);
	}
    allocateTemporary(t);
	return t;
}

antlrcpp::Any VariableVisitor::visitPost_decr(ifccParser::Post_decrContext *ctx)
{
	Type t = currentState().varTable[resolveVisibleVarSymbol(ctx->VAR()->getText())].type;
	if (t == Type::DOUBLE) {
		allocateTemporary(t);
	}
    allocateTemporary(t);
	return t;
}

antlrcpp::Any VariableVisitor::visitWhile_conditional(ifccParser::While_conditionalContext *ctx)
{
	loopLevel++;
	visitChildren(ctx);
	loopLevel--;
	return 0;
}

std::any VariableVisitor::visitSwitch_stmt(ifccParser::Switch_stmtContext *ctx)
{
    switchLevel++;
    visitChildren(ctx);
    switchLevel--;
    return 0;
}

antlrcpp::Any VariableVisitor::visitSwitch_case(ifccParser::Switch_caseContext *ctx)
{
    allocateTemporary(Type::INT);
    allocateTemporary(Type::INT);
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any VariableVisitor::visitStmt(ifccParser::StmtContext *ctx)
{
	if ((loopLevel == 0 && ctx->CONTINUE() != nullptr) || (loopLevel == 0 && ctx->BREAK() != nullptr && switchLevel == 0)) {
		if (debug)
			reportError("ERREUR : Une instruction réservée aux boucles est utilisée hors d'une boucle.");
	} else {
		visitChildren(ctx);
	}
	return 0;
}
std::any VariableVisitor::visitVar(ifccParser::VarContext *ctx)
{
    std::string var = ctx->VAR()->getText();
    std::string symbol = resolveVisibleVarSymbol(var);
    if (symbol.empty())
    {
        reportError("La variable " + var + " est utilisee avant declaration.");
        return Type::INT;
    }

    if (!currentState().varTable[symbol].affected)
    {
        reportError("La variable " + var + " est utilisee avant affectation.");
        return Type::INT;
    }

    currentState().varTable[symbol].used = true;
    if (debug)
    {
        std::cout << "utilisation de la variable " << var << " (" << symbol << ")" << std::endl;
    }
    return currentState().varTable[symbol].type;
}

std::any VariableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    if (currentReturnType == Type::VOID)
    {
        if (ctx->expression() != nullptr)
        {
            evalExpr(ctx->expression());
            reportError("Une fonction void ne peut pas retourner de valeur.");
        }
        hasReturn = true;
        return Type::VOID;
    }

    if (ctx->expression() == nullptr)
    {
        reportError("La fonction " + currentFunction + " doit retourner une valeur int.");
        hasReturn = true;
        return Type::INT;
    }

    Type exprType = evalExpr(ctx->expression());
    if (exprType == Type::VOID)
    {
        reportError("Impossible de retourner une expression de type void.");
    }
    if (exprType == Type::DOUBLE) {
    	allocateTemporary(Type::INT);
    }

    hasReturn = true;
    return exprType;
}

std::any VariableVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    (void)ctx;
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitDconst(ifccParser::DconstContext *ctx)
{
    (void)ctx;
    allocateTemporary(Type::DOUBLE);
    return Type::DOUBLE;
}

std::any VariableVisitor::visitPar(ifccParser::ParContext *ctx)
{
    return evalExpr(ctx->expression());
}

std::any VariableVisitor::visitCharconst(ifccParser::CharconstContext *ctx)
{
    (void)ctx;
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
{
    Type t = evalExpr(ctx->expression());
    if (t == Type::VOID)
    {
        reportError("L'operateur unaire - attend une expression non void.");
    }
    
    if (t == Type::DOUBLE) {
    	allocateTemporary(t);
    }
    allocateTemporary(t);
    return t;
}

std::any VariableVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations arithmetiques attendent des expressions non voids.");
    }
    Type outType = lhs == Type::DOUBLE || rhs == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    
    if (outType == Type::DOUBLE && (lhs == Type::INT || rhs == Type::INT)) {
    	allocateTemporary(outType); // conversion
    }
    allocateTemporary(outType);
    return outType;
}

std::any VariableVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations arithmetiques attendent des expressions non voids.");
    } else if (ctx->op->getText() == "%" && !(lhs == Type::INT && rhs == Type::INT))
    {
    	reportError("L'operation modulo attend des expressions entieres");
    }
    Type outType = lhs == Type::DOUBLE || rhs == Type::DOUBLE ? Type::DOUBLE : Type::INT;
    
    if (outType == Type::DOUBLE && (lhs == Type::INT || rhs == Type::INT)) {
    	allocateTemporary(outType); // conversion
    }
    allocateTemporary(outType);
    return outType;
}

std::any VariableVisitor::visitEq(ifccParser::EqContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les comparaisons attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitComp(ifccParser::CompContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les comparaisons attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitNot(ifccParser::NotContext *ctx)
{
    Type t = evalExpr(ctx->expression());
    if (t == Type::VOID)
    {
        reportError("L'operateur ! attend une expression entiere.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitBitwise_and(ifccParser::Bitwise_andContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations bit-a-bit attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitBitwise_xor(ifccParser::Bitwise_xorContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations bit-a-bit attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitBitwise_or(ifccParser::Bitwise_orContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations bit-a-bit attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitLogical_and(ifccParser::Logical_andContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations logiques attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitLogical_or(ifccParser::Logical_orContext *ctx)
{
    Type lhs = evalExpr(ctx->expression(0));
    Type rhs = evalExpr(ctx->expression(1));
    if (lhs == Type::VOID || rhs == Type::VOID)
    {
        reportError("Les operations logiques attendent des expressions entieres.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitGetchar(ifccParser::GetcharContext *ctx)
{
    (void)ctx;
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitPutchar(ifccParser::PutcharContext *ctx)
{
    Type argType = evalExpr(ctx->expression());
    if (argType == Type::VOID)
    {
        reportError("putchar attend un argument entier.");
    }
    allocateTemporary(Type::INT);
    return Type::INT;
}

std::any VariableVisitor::visitCall(ifccParser::CallContext *ctx)
{
    std::string functionName = ctx->VAR()->getText();
    auto it = functionSignatures.find(functionName);
    if (it == functionSignatures.end())
    {
        reportError("La fonction " + functionName + " n'est pas definie.");
        return Type::INT;
    }

    std::vector<Type> args;
    if (ctx->arg_list() != nullptr)
    {
        for (ifccParser::ExpressionContext *argExpr : ctx->arg_list()->expression())
        {
            args.push_back(evalExpr(argExpr));
        }
    }

    const FunctionSignature &sig = it->second;
    if (args.size() != sig.paramTypes.size())
    {
        reportError("La fonction " + functionName + " attend " + std::to_string(sig.paramTypes.size()) + " argument(s), mais " + std::to_string(args.size()) + " ont ete fournis.");
    }

    size_t common = std::min(args.size(), sig.paramTypes.size());
    for (size_t i = 0; i < common; ++i)
    {
        if (args[i] == Type::VOID || sig.paramTypes[i] != args[i])
        {
            reportError("Type incompatible pour l'argument " + std::to_string(i + 1) + " de " + functionName + ".");
        }
    }

    if (sig.returnType != Type::VOID)
    {
        allocateTemporary(Type::INT);
    }
    return sig.returnType;
}

