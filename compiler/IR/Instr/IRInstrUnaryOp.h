#pragma once

#include "BaseIRInstr.h"

class IRInstrUnaryOp : public BaseIRInstr
{

public:
    IRInstrUnaryOp(BasicBlock *bb_, string uniqueOp, string op)
        : BaseIRInstr(bb_), uniqueOp(uniqueOp), op(op) {}

    virtual void gen_asm(ostream &o);

protected:
    string uniqueOp;
    string op;

private:
    void handleNot(ostream &o);
    void handleNeg(ostream &o);
    void handleCompl(ostream &o);
    // void handlePost(ostream &o);
    // void handlePre(ostream &o);
};
