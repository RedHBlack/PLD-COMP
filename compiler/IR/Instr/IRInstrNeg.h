#pragma once

#include "IRInstrUnaryOp.h"

class IRInstrNeg : public IRInstrUnaryOp
{
public:
    IRInstrNeg(BasicBlock *bb_, string src)
        : IRInstrUnaryOp(bb_, src) {}

    virtual void gen_asm(ostream &o) override;
};