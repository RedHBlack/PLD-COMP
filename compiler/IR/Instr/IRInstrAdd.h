#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrAdd : public IRInstrBinaryOp
{
public:
    IRInstrAdd(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(ostream &o) override;
};