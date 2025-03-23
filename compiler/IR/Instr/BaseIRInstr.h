#pragma once

#include "../BasicBlock.h"
#include <string>
#include <ostream>

using namespace std;

class BasicBlock;

class BaseIRInstr
{
public:
    BaseIRInstr(BasicBlock *bb_) : bb(bb_) {};
    BasicBlock *getBB();
    virtual void gen_asm(ostream &o) = 0;

private:
    BasicBlock *bb;
};