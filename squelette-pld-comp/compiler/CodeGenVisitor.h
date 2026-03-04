#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"

#include <map>
#include <string>


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
		CodeGenVisitor(std::map<std::string, varInfo> table) : varTable(table) {}
		virtual ~CodeGenVisitor() {}
		virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
		virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
		virtual std::any visitConst(ifccParser::ConstContext *ctx) override;
		virtual std::any visitVar(ifccParser::VarContext *ctx) override;
		virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
	protected:
		std::map<std::string, varInfo> varTable;
};

