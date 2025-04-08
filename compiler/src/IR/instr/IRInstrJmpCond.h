#pragma once

#include "BaseIRInstr.h"

class IRInstrJmpCond : public BaseIRInstr
{
public:
    IRInstrJmpCond(BasicBlock *bb, string condition, string label, string reg)
        : BaseIRInstr(bb), condition(condition), label(label), reg(reg) {}

    virtual void gen_asm(ostream &o) override;

private:
    string condition; // Je, jne, jge, etc.
    string label;     // Destination du saut
    string reg;       // Registre à tester
};