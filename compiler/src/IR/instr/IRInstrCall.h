#pragma once

#include "BaseIRInstr.h"
#include <vector>
#include <string>

class IRInstrCall : public BaseIRInstr
{
public:
    IRInstrCall(BasicBlock *bb, const string funcName)
        : BaseIRInstr(bb), funcName(funcName) {}

    void gen_asm(ostream &o) override;

private:
    string funcName;
};
