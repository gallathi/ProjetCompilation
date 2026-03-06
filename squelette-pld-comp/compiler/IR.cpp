#include "IR.h"
#include <iostream>

void IRInstr::gen_asm(ostream &o) {
    switch(op) {
        case ldconst:
            o << "    movl $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << std::endl;
            break;
        case copy:
            o << "    movl $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << std::endl;
            break;
        case add:
            break;
        case sub:
            break;
        case mul:
            break;
        case rmem:
            break;
        case wmem:
            break;
        case call:
            break;
        case cmp_eq:
            break;
        case cmp_lt:
            break;
        case cmp_le:
            break;
    }
}