#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"
#include "IR.h"

#include <map>
#include <string>

class CodeGenVisitor : public ifccBaseVisitor
{
public:
	CodeGenVisitor(std::map<std::string, varInfo> table, int index) : varTable(table), nextIndex(index) {}
	virtual ~CodeGenVisitor() {}
	virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
	virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
	virtual std::any visitConst(ifccParser::ConstContext *ctx) override;
	virtual std::any visitVar(ifccParser::VarContext *ctx) override;
	virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
	virtual std::any visitAddsub(ifccParser::AddsubContext *ctx) override;
	virtual std::any visitMuldiv(ifccParser::MuldivContext *ctx) override;
	virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
	virtual std::any visitBlock(ifccParser::BlockContext *ctx);
	virtual std::any visitConditional(ifccParser::ConditionalContext *ctx);
	virtual std::any visitElse(ifccParser::ElseContext *ctx);
	virtual std::any visitElse_if(ifccParser::Else_ifContext *ctx);

protected:
	std::map<std::string, varInfo> varTable;
	int nextIndex;
};
