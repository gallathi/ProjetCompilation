#include "IR.h"
#include <iostream>
using namespace std;

void CFG::set_function_name(const string &name)
{
    functionName = name;
    returnLabel = name + "_return_exit";
}

string CFG::get_return_label() const
{
    return returnLabel;
}

void CFG::gen_asm_prologue(ostream &o, int compteurVar, bool emitGlobal)
{
#ifdef __APPLE__
    if (emitGlobal)
    {
        o << ".globl _" << functionName << "\n";
    }
    o << " _" << functionName << ": \n";
#else
    if (emitGlobal)
    {
        o << ".globl " << functionName << "\n";
    }
    o << functionName << ": \n";
#endif

    o << "    pushq %rbp\n";
    o << "    movq %rsp, %rbp\n";
    o << "    subq $" << compteurVar << ", %rsp" << endl;
}

void CFG::gen_asm_epilogue(ostream &o, bool emitStackNote)
{
    o << returnLabel << ":" << endl;
    o << "    movq %rbp, %rsp\n";
    o << "    popq %rbp\n";
    o << "    ret\n";

#ifndef __APPLE__
    if (emitStackNote)
    {
        o << ".section .note.GNU-stack,\"\",@progbits\n";
    }
#endif
}

void CFG::gen_asm(ostream &o)
{
    for (BasicBlock *bb : bbs)
    {
        bb->gen_asm(o);
    }
}

string CFG::IR_reg_to_asm(string reg)
{
    return "-" + to_string(SymbolIndex[reg]) + "(%rbp)";
}

void CFG::add_to_symbol_table(string name, Type t)
{
    SymbolType[name] = t;
    SymbolIndex[name] = nextFreeSymbolIndex;
    nextFreeSymbolIndex += 4;
}

string CFG::create_new_tempvar(Type t)
{
    string newName = "!tmp" + to_string(nextFreeSymbolIndex);
    return newName;
}

int CFG::get_var_index(string name)
{
    return SymbolIndex[name];
}

Type CFG::get_var_type(string name)
{
    return SymbolType[name];
}

string CFG::new_BB_name()
{
    string prefix = functionName.empty() ? "bb" : functionName + "_bb";
    string newName = prefix + to_string(nextBBnumber);
    ++nextBBnumber;
    return newName;
}

void CFG::add_bb(BasicBlock *bb)
{
    bbs.push_back(bb);
}

CFG::~CFG()
{
    for (BasicBlock *bb : bbs)
    {
        delete bb;
    }
}

BasicBlock::~BasicBlock()
{
    for (IRInstr *instr : instrs)
    {
        delete instr;
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params)
{
    instrs.push_back(new IRInstr(this, op, t, params));
}

void BasicBlock::gen_asm(ostream &o)
{
    o << label << ":" << endl;
    for (auto instr : instrs)
    {
        instr->gen_asm(o);
    }
    if (exit_true != nullptr)
    {
        if (exit_false != nullptr)
        {
            o << "    movl " << cfg->IR_reg_to_asm(test_var_name) << ", %eax" << endl;
            o << "    testl %eax, %eax" << endl;
            o << "    jz " << exit_false->label << endl;
        }
        o << "    jmp " << exit_true->label << endl;
    }
}

IRInstr::IRInstr(BasicBlock *bb, Operation op, Type t, vector<string> params) : bb(bb), op(op), t(t), params(params) {}

void IRInstr::gen_asm(ostream &o)
{
    string regs32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    switch (op)
    {
    case ldconst:
        o << "    movl $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case copy:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case add:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case sub:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    subl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case neg:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    negl %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case bool_not:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    test %eax, %eax" << endl;
        o << "    sete %al" << endl;
        o << "    mov %al, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case mul:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    imull " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case rmem:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        if (params.size() == 3)
        {
            o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        }
        o << "    movl (%eax), %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case wmem:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax" << endl;
        if (params.size() == 3)
        {
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %ebx" << endl;
            o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
            o << "    movl %eax, (%ebx)" << endl;
        }
        else
        {
            o << "    movl %eax, (" << bb->cfg->IR_reg_to_asm(params[1]) << ")" << endl;
        }
        break;
    case call:
    {
        for (int i = 0; i < params.size() - 2 && i < 6; i++)
        {
            o << "    movl " << bb->cfg->IR_reg_to_asm(params[i + 2]) << ", " << regs32[i] << endl;
        }

        int extraArgs = static_cast<int>(params.size()) - 8;
        if (extraArgs > 0)
        {
            for (int i = static_cast<int>(params.size()) - 1; i >= 8; --i)
            {
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[i]) << ", %eax" << endl;
                o << "    pushq %rax" << endl;
            }
        }

        o << "    call " << params[0] << endl;

        if (extraArgs > 0)
        {
            o << "    addq $" << (extraArgs * 8) << ", %rsp" << endl;
        }

        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[1]) << endl;
        break;
    }
    case load_param:
    {
        int paramIndex = stoi(params[1]);
        if (paramIndex < 6)
        {
            o << "    movl " << regs32[paramIndex] << ", " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        }
        else
        {
            int stackOffset = 16 + ((paramIndex - 6) * 8);
            o << "    movl " << stackOffset << "(%rbp), %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        }
        break;
    }
    case cmp_eq:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    sete %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case cmp_neq:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    setne %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case cmp_lt:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    setl %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case cmp_elt:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    setle %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case cmp_gt:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    setg %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case cmp_egt:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cmpl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    setge %al" << endl;
        o << "    movzbl %al, %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case div:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cdq" << endl;
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ecx" << endl;
        o << "    idiv %ecx" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case mod:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    cdq" << endl;
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ecx" << endl;
        o << "    idiv %ecx" << endl;
        o << "    movl %edx, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case bitwise_and:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    and " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case bitwise_xor:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    xor " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case bitwise_or:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
        o << "    or " << bb->cfg->IR_reg_to_asm(params[2]) << ", %eax" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case return_instr:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[0]) << ", %eax" << endl;
        o << "    jmp " << bb->cfg->get_return_label() << endl;
        break;
    case getchar:
        o << "    call getchar@PLT" << endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    case putchar:
        o << "    movl " << bb->cfg->IR_reg_to_asm(params[0]) << ", %edi" << endl;
        o << "    call putchar@PLT" << endl;
        break;
    case jump: // destination
        o << "    jmp " << params[0] << endl;
        break;
    case pointeurAffect:
        o << "    leaq " << bb->cfg->IR_reg_to_asm(params[1]) << ", %rax" << endl;
        o << "    movq    %rax, " << bb->cfg->IR_reg_to_asm(params[0]) << endl;
        break;
    }
}
