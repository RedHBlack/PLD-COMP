#pragma once

#include "IRInstrUnaryOp.h"

class IRInstrNot : public IRInstrUnaryOp
{
public:
    IRInstrNot(BasicBlock *bb_, string src)
        : IRInstrUnaryOp(bb_, src) {}

    virtual void gen_asm(ostream &o) override;
};