#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrMult : public IRInstrBinaryOp
{
public:
    IRInstrMult(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    void gen_asm(ostream &o) override;
};
