#include "IR.h"
#include <iostream>

void CFG::gen_asm_prologue(ostream& o) {
	#ifdef __APPLE__
    o << ".globl _main\n" ;
    o << " _main: \n" ;
    #else
    o << ".globl main\n" ;
    o << " main: \n" ;
    #endif

	std::cout<< "    pushq %rbp\n";
	std::cout<< "    movq %rsp, %rbp\n";
}

void CFG::gen_asm_epilogue(ostream& o) {
	std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

    #ifndef __APPLE__
        std::cout << ".section .note.GNU-stack,\"\",@progbits\n";
    #endif
}

IRInstr::IRInstr(BasicBlock *bb_, Operation op, Type t, vector<string> params) : bb(bb_), op(op), t(t), params(params)
{
}
