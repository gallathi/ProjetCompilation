#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class CodeGenVisitor : public ifccBaseVisitor
{
public:
	CodeGenVisitor(const std::map<std::string, FunctionSemanticState> &functionStates,
		const std::map<std::string, FunctionSignature> &functionSignatures)
		: functionStates(functionStates), functionSignatures(functionSignatures) {}
	virtual ~CodeGenVisitor() {}
	virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
	virtual std::any visitFunction_def(ifccParser::Function_defContext *ctx) override;
	virtual std::any visitParam(ifccParser::ParamContext *ctx) override;
	virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
	virtual std::any visitAffectation_declaration(ifccParser::Affectation_declarationContext *ctx) override;
	virtual std::any visitConst(ifccParser::ConstContext *ctx) override;
	virtual std::any visitVar(ifccParser::VarContext *ctx) override;
	virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
	virtual std::any visitAddsub(ifccParser::AddsubContext *ctx) override;
	virtual std::any visitMuldiv(ifccParser::MuldivContext *ctx) override;
	virtual std::any visitComp(ifccParser::CompContext *ctx) override;
	virtual std::any visitEq(ifccParser::EqContext *ctx) override;
	virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
	virtual std::any visitBlock(ifccParser::BlockContext *ctx);
	virtual std::any visitBlockNoAutoGen(ifccParser::BlockContext *ctx);
	virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override;
	virtual std::any visitPar(ifccParser::ParContext *ctx) override;
	virtual std::any visitNot(ifccParser::NotContext *ctx) override;
	virtual std::any visitBitwise_and(ifccParser::Bitwise_andContext *ctx) override;
	virtual std::any visitBitwise_xor(ifccParser::Bitwise_xorContext *ctx) override;
	virtual std::any visitBitwise_or(ifccParser::Bitwise_orContext *ctx) override;
	virtual std::any visitCharconst(ifccParser::CharconstContext *ctx) override;
	virtual std::any visitPutchar(ifccParser::PutcharContext *ctx) override;
	virtual std::any visitGetchar(ifccParser::GetcharContext *ctx) override;
	virtual std::any visitCall(ifccParser::CallContext *ctx) override;

protected:
	std::map<std::string, FunctionSemanticState> functionStates;
	std::map<std::string, FunctionSignature> functionSignatures;
	std::string currentFunction;
	std::map<std::string, varInfo> currentVarTable;
	int currentFrameBytes = 0;
	bool hasReturned = false;
	int declarationCounter = 0;
	int currentParamIndex = 0;
	std::vector<std::unordered_map<std::string, std::string>> scopeStack;

	std::string createScopedName(const std::string &name);
	std::string resolveVisibleVar(const std::string &name) const;
	void bindFunctionState(const std::string &name);
};
