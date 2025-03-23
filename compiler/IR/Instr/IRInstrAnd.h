#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrAnd : public IRInstrBinaryOp
{
public:
    IRInstrAnd(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(ostream &o) override;
};