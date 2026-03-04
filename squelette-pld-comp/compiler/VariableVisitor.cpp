#include "VariableVisitor.h"

#include <iostream>

int VariableVisitor::getErrorCount() {
	return errorCount;
}

int VariableVisitor::getNextOffset() {
    return nextIndex;
}

std::map<std::string, varInfo> VariableVisitor::getVarTable() {
	return varTable;
}

antlrcpp::Any VariableVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx) {
	std::string var = ctx->VAR()->getText();
	if (varTable.find(var) != varTable.end()) {
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est déclarée plusieurs fois." << std::endl;
		errorCount++;
		return 0;
	}
	varInfo info;
	info.index = nextIndex;
	nextIndex += 4;
	info.used = false;
    varTable[var] = info;
    if (debug)
    	std::cout << "déclaration de " << var << " : " << info.index << std::endl;
    if (ctx->declaration_var() != nullptr)
    	visit(ctx->declaration_var());
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    visit(ctx->expression());
    std::string var = ctx->VAR()->getText();
	if (varTable.find(var) == varTable.end()) {
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant déclaration." << std::endl;
		errorCount++;
		return 0;
	}
    varTable[var].used = true;
    if (debug)
    	std::cout << "affectation de la variable " << var << std::endl;
    return 0;
}

antlrcpp::Any VariableVisitor::visitVar(ifccParser::VarContext *ctx) {
	std::string var = ctx->VAR()->getText();
	if (varTable.find(var) == varTable.end()) {
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant déclaration." << std::endl;
		errorCount++;
		return 0;
	}
	varTable[var].used = true;
	if (debug)
    	std::cout << "utilisation de la variable " << var << std::endl;
    return 0;
}

antlrcpp::Any VariableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
	visit(ctx->expression());
	bool allUsed = true;
	for (std::map<std::string, varInfo>::iterator it = varTable.begin(); it != varTable.end(); it++) {
		if (!it->second.used) {
			allUsed = false;
		}
	}
	
	if (!allUsed) {
		if (debug)
			std::cout << "ERREUR : Toutes les variables déclarées n'ont pas été utilisées" << std::endl;
		errorCount++;
	}
    return 0;
}

