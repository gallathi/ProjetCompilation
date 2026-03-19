#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"
#include "IR.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

class CodeGenVisitor : public ifccBaseVisitor
{
public:
	CodeGenVisitor(std::map<std::string, varInfo> table, int index, int compteurVar) : varTable(table), nextIndex(index), compteurVar(compteurVar) {}
	virtual ~CodeGenVisitor() {}
	virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
	virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
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
	virtual std::any visitConditional(ifccParser::ConditionalContext *ctx);
	virtual std::any visitElse(ifccParser::ElseContext *ctx);
	virtual std::any visitElse_if(ifccParser::Else_ifContext *ctx);
	virtual std::any visitWhile_conditional(ifccParser::While_conditionalContext *ctx);
	virtual std::any visitPre_incr(ifccParser::Pre_incrContext *ctx) override;
	virtual std::any visitPre_decr(ifccParser::Pre_decrContext *ctx) override;
	virtual std::any visitPost_incr(ifccParser::Post_incrContext *ctx) override;
	virtual std::any visitPost_decr(ifccParser::Post_decrContext *ctx) override;
	virtual std::any visitStmt(ifccParser::StmtContext *ctx) override;

protected:
	std::map<std::string, varInfo> varTable;
	int nextIndex;
	int compteurVar;
	std::map<std::string, std::string> postfixOps;
	bool hasReturned = false;
	int declarationCounter = 0;
	std::vector<std::unordered_map<std::string, std::string>> scopeStack;
	std::vector<std::pair<BasicBlock*, BasicBlock*>> loopStack;

	std::string createScopedName(const std::string &name);
	std::string resolveVisibleVar(const std::string &name) const;
};
