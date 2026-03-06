#include "IR.h"
#include <iostream>

using namespace std;

void CFG::gen_asm_prologue(ostream& o) {
	#ifdef __APPLE__
    o << ".globl _main\n" ;
    o << " _main: \n" ;
    #else
    o << ".globl main\n" ;
    o << " main: \n" ;
    #endif

	o << "    pushq %rbp\n";
	o << "    movq %rsp, %rbp\n";
}

void CFG::gen_asm_epilogue(ostream& o) {
	o << "    popq %rbp\n";
    o << "    ret\n";

    #ifndef __APPLE__
        o << ".section .note.GNU-stack,\"\",@progbits\n";
    #endif
}

void CFG::gen_asm(ostream &o) {
	current_bb = bbs[0];
	current_bb->gen_asm(o);
	while (current_bb->exit_true != nullptr) {
		current_bb = current_bb->exit_true;
	}
}

string CFG::IR_reg_to_asm(string reg) {
	return "-" + itos(SymbolIndex[reg]) + "%(rbp)";
}

void CFG::add_to_symbol_table(string name, Type t) {
	SymbolType[name] = t;
	SymbolIndex[name] = nextFreeSymbolIndex;
	nextFreeSymbolIndex++;
}

string CFG::create_new_tempvar(Type t) {
	string newName = "!tmp" + itos(nextFreeSymbolIndex);
	return newName;
}

int CFG::get_var_index(string name) {
	return SymbolIndex[name];
}

Type CFG::get_var_type(string name) {
	return SymbolType[name];
}

string CFG::new_BB_name() {
	string newName = "bb" + itos(nextBBnumber);
	return newName; 
}

void CFG::add_bb(BasicBlock *bb) {
	bbs.push_back(bb);
}

IRInstr::IRInstr(BasicBlock *bb_, Operation op, Type t, vector<string> params) : bb(bb_), op(op), t(t), params(params)
{
}
