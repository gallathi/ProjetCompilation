#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>
#include <string>
#include <iostream>

class VariableVisitor : public ifccBaseVisitor {
public:
    VariableVisitor(bool debug) : debug(debug) {};
    virtual ~VariableVisitor() {};
    virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override;
    virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override;
    virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual std::any visitVar(ifccParser::VarContext *ctx) override;
    int getErrorCount();
    int getPosition(std::string variable);
    int getNumberVar();
protected:
    struct VarInfo {
        int index;
        bool used;
    };
    std::map<std::string, VarInfo> varTable;
    int nextIndex = 4;
    int errorCount = 0;
    bool debug;
};