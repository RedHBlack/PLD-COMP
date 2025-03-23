#pragma once

#include "BaseIRInstr.h"

class IRInstrBinaryOp : public BaseIRInstr
{

public:
    IRInstrBinaryOp(BasicBlock *bb_, string src, string dest)
        : BaseIRInstr(bb_), src(src), dest(dest) {}

    virtual void gen_asm(ostream &o) = 0;

protected:
    string src;
    string dest;
};
