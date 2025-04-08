#include "IRInstrLoadFromArray.h"

IRInstrLoadFromArray::IRInstrLoadFromArray(BasicBlock *bb, string src, string dest, int index)
    : BaseIRInstr(bb), src(src), dest(dest), index(index)
{
}

void IRInstrLoadFromArray::gen_asm(ostream &o)
{
    if (index < 0)
    {
        // Cas d'un index dynamique : le résultat de l'évaluation se trouve dans %rax
        o << "   cltq\n";
        const int baseOffset = this->symbolsTable->getSymbolIndex(src);
        o << "   movl " << baseOffset << "(%rbp,%rax,4), " << dest << "\n";
    }
    else
    {
        // Cas d'un index constant.
        const int baseOffset = this->symbolsTable->getSymbolIndex(src);
        const int offsetDestinationVar = baseOffset + index * 4;
        o << "   movl " << offsetDestinationVar << "(%rbp), " << dest << "\n";
    }
}
