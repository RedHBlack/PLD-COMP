#pragma once

#include "BaseIRInstr.h"

class IRInstrUnaryOp : public BaseIRInstr
{

public:
    IRInstrUnaryOp(BasicBlock *bb_, string src)
        : BaseIRInstr(bb_), src(src) {}

    virtual void gen_asm(ostream &o) = 0;

protected:
    string src;
};
