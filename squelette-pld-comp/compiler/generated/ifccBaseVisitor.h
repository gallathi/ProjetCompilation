
// Generated from ifcc.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ifccVisitor.h"


/**
 * This class provides an empty implementation of ifccVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ifccBaseVisitor : public ifccVisitor {
public:

  virtual std::any visitAxiom(ifccParser::AxiomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProg(ifccParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBloc(ifccParser::BlocContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(ifccParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration_var(ifccParser::Declaration_varContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAffectation(ifccParser::AffectationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPar(ifccParser::ParContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConst(ifccParser::ConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVar(ifccParser::VarContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddsub(ifccParser::AddsubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMuldiv(ifccParser::MuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override {
    return visitChildren(ctx);
  }


};

