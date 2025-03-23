#pragma once

#include "BaseIRInstr.h"
#include "IRInstrBinaryOp.h"

class IRInstrArithmeticOp : public IRInstrBinaryOp
{

public:
    IRInstrArithmeticOp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    virtual void gen_asm(ostream &o);

private:
    void handleAddition(ostream &o);
    void handleSubstraction(ostream &o);
    void handleMult(ostream &o);
    void handleDiv(ostream &o);
    void handleModulo(ostream &o);
    void handleBitwiseAnd(ostream &o);
    void handleBitwiseOr(ostream &o);
    void handleBitwiseXor(ostream &o);
};
