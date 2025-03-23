#pragma once

#include "IRInstrBinaryOp.h"
#include <ostream>

class IRInstrComp : public IRInstrBinaryOp
{
public:
    // Constructor for the comparison instruction
    IRInstrComp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    // Override the gen_asm function to generate the assembly code for comparisons
    virtual void gen_asm(ostream &o) override;

private:
    void handleCompEq(ostream &o);
    void handleCompNotEq(ostream &o);
    void handleGreater(ostream &o);
    void handleGreaterOrEqual(ostream &o);
    void handleLower(ostream &o);
    void handleLowerOrEqual(ostream &o);
};
