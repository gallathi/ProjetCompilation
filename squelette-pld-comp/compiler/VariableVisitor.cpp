#include "VariableVisitor.h"

int VariableVisitor::getErrorCount()
{
    return errorCount;
}

int VariableVisitor::getPosition(std::string variable) {
    return varTable[variable].index;
}

int VariableVisitor::getNumberVar() {
    return varTable.size();
}

antlrcpp::Any VariableVisitor::visitDeclaration_var(ifccParser::Declaration_varContext *ctx)
{
    std::string var = ctx->VAR()->getText();
    if (varTable.find(var) != varTable.end()) {
        std::cout << "Erreur: variable '" + var + "' déclarée plusieurs fois." << std::endl;
        errorCount++;
        return 0;
    }

    VarInfo infos;
    infos.index = nextIndex;
    nextIndex += 4;
    infos.used = false;
    varTable[var] = infos;
    if (debug) {
        std::cout << "La variable '" + var + "' a été déclarée en ";
        std::cout << (nextIndex-4);
        std::cout << "." << std::endl;
    }
    if (ctx->declaration_var() != nullptr) {
        visitDeclaration_var(ctx->declaration_var());
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    std::string var = ctx->VAR()->getText();
    visit(ctx->expression());

    if (varTable.find(var) == varTable.end()) {
        std::cout << "Erreur: variable '" + var + "' non déclarée avant utilisation." << std::endl;
        errorCount++;
        return 0;
    }

    varTable[var].used = true;
    if (debug) {
        std::cout << "La variable '" + var + "' a été utilisée." << std::endl;
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitVar(ifccParser::VarContext *ctx)
{
    std::string var = ctx->VAR()->getText();

    if (varTable.find(var) == varTable.end()) {
        std::cout << "Erreur: variable '" + var + "' non déclarée avant utilisation." << std::endl;
        errorCount++;
        return 0;
    }

    varTable[var].used = true;
    if (debug) {
        std::cout << "La variable '" + var + "' a été utilisée." << std::endl;
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    for (const auto& [var, varInfos] : varTable) {
        if (!varInfos.used) {
            std::cout << "Erreur: variable '" + var + "' déclarée mais jamais utilisée." << std::endl;
            errorCount++;
        }
    }
    if (debug) {
        std::cout << "Fin du programme." << std::endl;
    }
    return 0;
}