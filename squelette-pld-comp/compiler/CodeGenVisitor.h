#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"

#include <map>
#include <string>

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
	virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
	virtual std::any visitBlock(ifccParser::BlockContext *ctx);
	virtual std::any visitPar(ifccParser::ParContext *ctx) override;
	virtual std::any visitNot(ifccParser::NotContext *ctx) override;

protected:
	std::map<std::string, varInfo> varTable;
	int nextIndex;
	int compteurVar;
};
