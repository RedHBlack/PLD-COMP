#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrMod : public IRInstrBinaryOp
{
public:
    IRInstrMod(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(ostream &o) override;
};
