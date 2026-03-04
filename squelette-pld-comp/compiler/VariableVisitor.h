#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>
#include <string>


struct varInfo {
	int index;
	bool used;
};

class  VariableVisitor : public ifccBaseVisitor {
	public:
		VariableVisitor(bool debug) : debug(debug) {}
		virtual ~VariableVisitor() {}
		virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override;
		virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
		virtual std::any visitVar(ifccParser::VarContext *ctx) override;
		virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
		int getErrorCount();
		int getNextOffset();
		std::map<std::string, varInfo> getVarTable();
	protected:
		std::map<std::string, varInfo> varTable;
		int nextIndex = 4;
		int errorCount = 0;
		bool debug;
};

