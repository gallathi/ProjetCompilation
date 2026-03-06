
#include "IR.h"

IRInstr::IRInstr(BasicBlock *bb_, Operation op, Type t, vector<string> params) : bb(bb_), op(op), t(t), params(params)
{
}