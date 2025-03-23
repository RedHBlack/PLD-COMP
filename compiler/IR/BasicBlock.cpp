#include "BasicBlock.h"

BasicBlock::BasicBlock(CFG *cfg, string entry_label) : cfg(cfg), label(entry_label)
{
}

void BasicBlock::gen_asm(ostream &o)
{
    for (int i = 0; i < instrs.size(); i++)
    {
        instrs[i]->gen_asm(o);
    }
}

void BasicBlock::add_IRInstr(BaseIRInstr *instr)
{
    instrs.push_back(instr);
}

CFG *BasicBlock::getCFG()
{
    return this->cfg;
}
