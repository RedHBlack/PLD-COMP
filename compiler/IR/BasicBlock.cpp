#include "BasicBlock.h"

BasicBlock::BasicBlock(CFG *cfg, string entry_label) : cfg(cfg), label(entry_label), exit_true(nullptr), exit_false(nullptr)
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

string BasicBlock::getLabel()
{
    return label;
}

void BasicBlock::setLabel(string label)
{
    this->label = label;
}

vector<BaseIRInstr *> BasicBlock::getInstr()
{
    return instrs;
}

void BasicBlock::setExitTrue(BasicBlock *bb)
{
    exit_true = bb;
}

void BasicBlock::setExitFalse(BasicBlock *bb)
{
    exit_false = bb;
}

BasicBlock *BasicBlock::getExitTrue()
{
    return exit_true;
}

BasicBlock *BasicBlock::getExitFalse()
{
    return exit_false;
}
