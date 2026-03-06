#include "IR.h"
#include <iostream>

void CFG::gen_asm_prologue(ostream &o)
{
#ifdef __APPLE__
    o << ".globl _main\n";
    o << " _main: \n";
#else
    o << ".globl main\n";
    o << " main: \n";
#endif

    std::cout << "    pushq %rbp\n";
    std::cout << "    movq %rsp, %rbp\n";
}

void CFG::gen_asm_epilogue(ostream &o)
{
    std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

#ifndef __APPLE__
    std::cout << ".section .note.GNU-stack,\"\",@progbits\n";
#endif
}

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
