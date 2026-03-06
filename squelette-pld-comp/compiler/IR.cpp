
#include "IR.h"

IRInstr::IRInstr(BasicBlock *bb_, Operation op, Type t, vector<string> params) : bb(bb_), op(op), t(t), params(params)
{
}

BasicBlock::BasicBlock(CFG *cfg, string entry_label) : cfg(cfg), label(entry_label), exit_true(nullptr), exit_false(nullptr)
{
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params)
{
    instrs.push_back(new IRInstr(this, op, t, params));
}

CFG::CFG(DefFonction *ast) : ast(ast), nextFreeSymbolIndex(0), nextBBnumber(0), current_bb(nullptr)
{
}

string CFG::new_BB_name()
{
    return ast->getName() + "_BB" + to_string(nextBBnumber++);
}

void CFG::add_bb(BasicBlock *bb)
{
    bbs.push_back(bb);
}