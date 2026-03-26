#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "type.h"

#include <map>
#include <stack>
#include <string>
#include <vector>

struct varInfo
{
	int index;
	bool used;
	bool affected;
	Type type;
};

struct FunctionSignature
{
	Type returnType;
	std::vector<Type> paramTypes;
};

struct FunctionSemanticState
{
	std::map<std::string, varInfo> varTable;
	int nextIndex = 0;
	int compteurVar = 4;
	int declarationCounter = 0;
};

class VariableVisitor : public ifccBaseVisitor
{
public:
	VariableVisitor(bool debug) : debug(debug) {}
	virtual ~VariableVisitor() {}
	virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
	virtual std::any visitFunction_def(ifccParser::Function_defContext *ctx) override;
	virtual std::any visitParam_list(ifccParser::Param_listContext *ctx) override;
	virtual std::any visitParam(ifccParser::ParamContext *ctx) override;
	virtual std::any visitType(ifccParser::TypeContext *ctx) override;
	virtual std::any visitBlock(ifccParser::BlockContext *ctx) override;
	virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override;
	virtual std::any visitAffectation_declaration(ifccParser::Affectation_declarationContext *ctx) override;
	virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
	virtual std::any visitVar(ifccParser::VarContext *ctx) override;
	virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
	virtual std::any visitConst(ifccParser::ConstContext *ctx) override;
	virtual std::any visitPar(ifccParser::ParContext *ctx) override;
	virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
	virtual std::any visitAddsub(ifccParser::AddsubContext *ctx) override;
	virtual std::any visitMuldiv(ifccParser::MuldivContext *ctx) override;
	virtual std::any visitCharconst(ifccParser::CharconstContext *ctx) override;
	virtual std::any visitComp(ifccParser::CompContext *ctx) override;
	virtual std::any visitEq(ifccParser::EqContext *ctx) override;
	virtual std::any visitNot(ifccParser::NotContext *ctx) override;
	virtual std::any visitBitwise_and(ifccParser::Bitwise_andContext *ctx) override;
	virtual std::any visitBitwise_xor(ifccParser::Bitwise_xorContext *ctx) override;
	virtual std::any visitBitwise_or(ifccParser::Bitwise_orContext *ctx) override;
	virtual std::any visitGetchar(ifccParser::GetcharContext *ctx) override;
	virtual std::any visitPre_incr(ifccParser::Pre_incrContext *ctx) override;
	virtual std::any visitPre_decr(ifccParser::Pre_decrContext *ctx) override;
	virtual std::any visitPost_incr(ifccParser::Post_incrContext *ctx) override;
	virtual std::any visitPost_decr(ifccParser::Post_decrContext *ctx) override;
	virtual std::any visitStmt(ifccParser::StmtContext *ctx) override;
	virtual std::any visitWhile_conditional(ifccParser::While_conditionalContext *ctx) override;
	virtual std::any visitDecla_affect(ifccParser::Decla_affectContext *ctx) override;
	virtual std::any visitPutchar(ifccParser::PutcharContext *ctx) override;
	virtual std::any visitCall(ifccParser::CallContext *ctx) override;
	virtual std::any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
	virtual std::any visitDconst(ifccParser::DconstContext *ctx) override;

	int getErrorCount();
	int getNextOffset();
	int getCompteurVar();
	std::map<std::string, varInfo> getVarTable();
	const std::map<std::string, FunctionSignature> &getFunctionSignatures() const;
	const std::map<std::string, FunctionSemanticState> &getFunctionStates() const;

protected:
	struct ScopedVarInfo
	{
		std::string name;
		std::string symbol;
		int block;
	};

	std::stack<ScopedVarInfo> scopedVars;
	int scopeBlock = 0;

	void clearScopedVars();
	bool checkIfVarInCurrentBlock(const std::string &var) const;
	std::string resolveVisibleVarSymbol(const std::string &var) const;

	FunctionSemanticState &currentState();
	void reportError(const std::string &msg);
	Type parseTypeText(const std::string &text) const;
	Type evalExpr(ifccParser::ExpressionContext *ctx);
	std::string declareVar(const std::string &varName, bool affected);
	void allocateTemporary(Type t);

	std::map<std::string, FunctionSignature> functionSignatures;
	std::map<std::string, FunctionSemanticState> functionStates;
	std::string currentFunction;
	Type currentReturnType = Type::INT;
	int errorCount = 0;
	bool debug;
	bool hasReturn = false;
	int declarationCounter = 0;
	int loopLevel = 0;
	Type currentType;
};
