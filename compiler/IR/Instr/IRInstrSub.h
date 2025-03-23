#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrSub : public IRInstrBinaryOp
{
public:
    IRInstrSub(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(ostream &o) override;
};