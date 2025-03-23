#pragma once

#include "BaseIRInstr.h"

class IRInstrBinaryOp : public BaseIRInstr
{

public:
    IRInstrBinaryOp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : BaseIRInstr(bb_), firstOp(firstOp), secondOp(secondOp), op(op) {}

    virtual void gen_asm(ostream &o) = 0;

protected:
    string firstOp;
    string secondOp;
    string op;
};
