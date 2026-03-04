#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
	    CodeGenVisitor(VariableVisitor* vv) : varVisitor(vv) {};
        virtual ~CodeGenVisitor() {};
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
        virtual antlrcpp::Any visitConst(ifccParser::ConstContext *ctx) override;
        virtual antlrcpp::Any visitVar(ifccParser::VarContext *ctx) override;
    protected:
        VariableVisitor* varVisitor;
};

