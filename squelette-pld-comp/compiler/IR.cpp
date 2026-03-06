#include "IR.h"
#include <iostream>
using namespace std;

void IRInstr::gen_asm(ostream &o) {
    switch(op) {
        case ldconst: // Params : destination / const
            o << "    movl $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case copy: // Params : destination / variable à assigner
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case add: // Params : destination / var1 / var2
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case sub: // Params : destination / var qui va diminuer / var à retirer
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    subl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case mul: // Params : destination / var1 / var2
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    imull " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case rmem: // Params : destination / adresse à lire / offset potentiel (tableau)
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            if (params.size() == 3) {
                o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            }
            o << "    movl (%eax), %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case wmem: // Params : valeur / adresse à modifier / offset potentiel (tableau)
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax" << endl;
            if (params.size() == 3) {
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %ebx" << endl;
                o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
                o << "    movl %eax, (%ebx)" << endl;
            } else {
                o << "    movl %eax, (" << bb->cfg->IR_reg_to_asm(params[1]) << ")" << endl;
            }
            break;
        case call: // Params : nom de la fonction / variable qui va stocker le retour de la fonction / paramètres de la fonction
            string regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            for (int i = 0; i < params.size() - 2 && i < 6; i++) {
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[i+2]) << ", " << regs[i] << endl;
            }
            o << "    call " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[1]) << endl;
            break;
        case cmp_eq: // Params : destination / var1 / var2
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    sete %al" << endl;
            o << "    movzbl %al, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case cmp_lt: // Params : destination / var1 / var2 (on renvoie si var1 < var2)
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    setl %al" << endl;
            o << "    movzbl %al, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
        case cmp_le: // Params : destination / var1 / var2 (on renvoie si var1 > var2)
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
            o << "    setg %al" << endl;
            o << "    movzbl %al, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
            break;
    }
}