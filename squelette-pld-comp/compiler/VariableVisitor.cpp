#include "VariableVisitor.h"

#include <iostream>
#include <stack>
#include <utility>
#include <string>

std::stack<std::pair<std::string, int>> s;

int currentBlock;

bool checkIfVarInStack(std::string var)
{
	std::stack<std::pair<std::string, int>> tempStack = s;

	while (!tempStack.empty())
	{
		auto top = tempStack.top();

		if (top.first == var)
		{
			return true;
		}

		tempStack.pop();
	}

	return false;
}

int VariableVisitor::getErrorCount()
{
	return errorCount;
}

int VariableVisitor::getNextOffset()
{
	return nextIndex;
}

int VariableVisitor::getCompteurVar() {
	return compteurVar;
}

std::map<std::string, varInfo> VariableVisitor::getVarTable()
{
	return varTable;
}

antlrcpp::Any VariableVisitor::visitProg(ifccParser::ProgContext *ctx)
{

	visit(ctx->block());

	// vérifie variables non utilisées
	bool allUsed = true;
	for (std::map<std::string, varInfo>::iterator it = varTable.begin(); it != varTable.end(); it++)
	{
		if (!it->second.used)
		{
			allUsed = false;
		}
	}
	if (!allUsed)
	{
		if (debug)
			std::cout << "ERREUR : Toutes les variables déclarées n'ont pas été utilisées" << std::endl;
		errorCount++;
	}

	return 0;
}

antlrcpp::Any VariableVisitor::visitBlock(ifccParser::BlockContext *ctx)
{

	currentBlock++;
	for (auto s : ctx->stmt())
	{
		visit(s);
	}
	// je veux iterer sur la stack pour enlever les variable du precedant block
	while (!s.empty() && s.top().second == currentBlock)
	{
		s.pop();
	}
	currentBlock--;
	return 0;
}
antlrcpp::Any VariableVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx)
{
	std::string var = ctx->VAR()->getText();
	if (varTable.find(var) != varTable.end())
	{
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est déclarée plusieurs fois." << std::endl;
		errorCount++;
		return 0;
	}
	varInfo info;
	info.index = nextIndex;
	nextIndex += 4;
	compteurVar += 4;
	info.used = false;
	info.affected = false;
	varTable[var] = info;

	s.push(std::make_pair(var, currentBlock));

	if (debug)
		std::cout << "déclaration de " << var << " : " << info.index << std::endl;
	if (ctx->declaration_var() != nullptr)
		visit(ctx->declaration_var());
	return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	visit(ctx->expression());
	std::string var = ctx->VAR()->getText();
	std::string op = ctx->op->getText();
	if (varTable.find(var) == varTable.end())
	{
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant déclaration." << std::endl;
		errorCount++;
		return 0;
	}

	bool declaredInEnclosingBlock = checkIfVarInStack(var);

	if (!declaredInEnclosingBlock)
	{
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant déclaration." << std::endl;
		errorCount++;
		return 0;
	}

	varTable[var].used = true;
	varTable[var].affected = true;

	if (debug)
		std::cout << "affectation de la variable " << var << std::endl;
	return 0;
}

antlrcpp::Any VariableVisitor::visitPre_incr(ifccParser::Pre_incrContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitPre_decr(ifccParser::Pre_decrContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitPost_incr(ifccParser::Post_incrContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitPost_decr(ifccParser::Post_decrContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitVar(ifccParser::VarContext *ctx)
{
	std::string var = ctx->VAR()->getText();
	if (varTable.find(var) == varTable.end())
	{
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant déclaration." << std::endl;
		errorCount++;
		return 0;
	}
	else if (!varTable[var].affected)
	{
		if (debug)
			std::cout << "ERREUR : La variable " << var << " est utilisée avant afféctation." << std::endl;
		errorCount++;
		return 0;
	}

	bool declaredInEnclosingBlock = checkIfVarInStack(var);

	if (!declaredInEnclosingBlock)
	{
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

antlrcpp::Any VariableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{

	visit(ctx->expression());
	bool allUsed = true;

	return 0;
}
antlrcpp::Any VariableVisitor::visitConst(ifccParser::ConstContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitCharconst(ifccParser::CharconstContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitEq(ifccParser::EqContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitComp(ifccParser::CompContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitNot(ifccParser::NotContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitGetchar(ifccParser::GetcharContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitBitwise_xor(ifccParser::Bitwise_xorContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitBitwise_or(ifccParser::Bitwise_orContext *ctx)
{
	compteurVar += 4;
	return 0;
}

antlrcpp::Any VariableVisitor::visitBitwise_and(ifccParser::Bitwise_andContext *ctx)
{
	compteurVar += 4;
	return 0;
}
