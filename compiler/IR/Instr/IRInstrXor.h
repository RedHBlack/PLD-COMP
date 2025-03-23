#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrXor : public IRInstrBinaryOp
{
public:
    IRInstrXor(BasicBlock *bb_, string src, string dest)
        : IRInstrBinaryOp(bb_, src, dest) {}

    virtual void gen_asm(std::ostream &o) override;
};
