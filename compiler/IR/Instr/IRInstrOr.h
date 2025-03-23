#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrOr : public IRInstrBinaryOp
{
public:
    IRInstrOr(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(ostream &o) override;
};