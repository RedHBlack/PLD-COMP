#include "BaseIRInstr.h"

BaseIRInstr::BaseIRInstr(BasicBlock *bb_)
{
    this->bb = bb_;
    this->symbolsTable = bb_->getCFG()->getSymbolsTable();
}

BasicBlock *BaseIRInstr::getBB()
{
    return this->bb;
}