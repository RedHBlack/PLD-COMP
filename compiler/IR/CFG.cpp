#include "CFG.h"

CFG::CFG(string label, map<string, int> SymbolIndex, int initialNextFreeSymbolIndex) : label(label), SymbolIndex(SymbolIndex), nextFreeSymbolIndex(initialNextFreeSymbolIndex), initialTempPos(initialNextFreeSymbolIndex)
{
}

void CFG::add_bb(BasicBlock *bb)
{
    bbs.push_back(bb);
    this->setCurrentBasicBlock(bb);
}

void CFG::gen_asm(ostream &o)
{
    gen_asm_prologue(o);

    for (int i = 0; i < bbs.size(); i++)
    {
        bbs[i]->gen_asm(o);
    }

    gen_asm_epilogue(o);
}

void CFG::gen_asm_prologue(ostream &o)
{
#ifdef __APPLE__
    o << ".globl _" << this->label "\n";
    o << " _" << this->label << ":\n";
#else
    o << ".globl " << this->label << "\n";
    o << this->label << ":\n";
#endif

    o << "   pushq %rbp\n";
    o << "   movq %rsp, %rbp\n";
}

void CFG::gen_asm_epilogue(ostream &o)
{

    o << "   popq %rbp\n";
    o << "   ret\n";
}

string CFG::create_new_tempvar()
{
    const string newTmpVar = "tmp" + to_string(-nextFreeSymbolIndex);

    this->SymbolIndex[newTmpVar] = nextFreeSymbolIndex;
    this->nextFreeSymbolIndex -= 4;

    return newTmpVar;
}

int CFG::get_var_index(string name)
{
    return this->SymbolIndex[name];
}

BasicBlock *CFG::getCurrentBasicBlock()
{
    return this->current_bb;
}

void CFG::setCurrentBasicBlock(BasicBlock *bb)
{
    this->current_bb = bb;
}

void CFG::resetNextFreeSymbolIndex()
{
    nextFreeSymbolIndex = initialTempPos;
}
