#pragma once

#include "IRInstrBinaryOp.h"
#include <iostream>

class IRInstrComp : public IRInstrBinaryOp
{
public:
    // Constructor for the comparison instruction
    IRInstrComp(BasicBlock *bb_, string src, string dest, string op)
        : IRInstrBinaryOp(bb_, src, dest), op(op) {}

    // Override the gen_asm function to generate the assembly code for comparisons
    virtual void gen_asm(ostream &o) override;

private:
    string op;
};
