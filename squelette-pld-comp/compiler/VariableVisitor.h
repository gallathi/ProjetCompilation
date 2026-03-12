#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>
#include <string>

struct varInfo
{
	int index;
	bool used;
	bool affected;
};

class VariableVisitor : public ifccBaseVisitor
{
public:
	VariableVisitor(bool debug) : debug(debug) {}
	virtual ~VariableVisitor() {}
	virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override;
	virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
	virtual std::any visitVar(ifccParser::VarContext *ctx) override;
	virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
	virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
	virtual std::any visitBlock(ifccParser::BlockContext *ctx) override;
	virtual std::any visitConst(ifccParser::ConstContext *ctx) override;
	virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
	virtual std::any visitAddsub(ifccParser::AddsubContext *ctx) override;
	virtual std::any visitMuldiv(ifccParser::MuldivContext *ctx) override;
	virtual std::any visitCharconst(ifccParser::CharconstContext *ctx) override;
    virtual std::any visitComp(ifccParser::CompContext *ctx) override;
    virtual std::any visitEq(ifccParser::EqContext *ctx) override;
    virtual std::any visitNot(ifccParser::NotContext *ctx) override;
    virtual std::any visitGetchar(ifccParser::GetcharContext *ctx) override;

	int getErrorCount();
	int getNextOffset();
	int getCompteurVar();
	std::map<std::string, varInfo> getVarTable();

protected:
	std::map<std::string, varInfo> varTable;
	int nextIndex = 0;
	int errorCount = 0;
	bool debug;
	int compteurVar = 0;
};
