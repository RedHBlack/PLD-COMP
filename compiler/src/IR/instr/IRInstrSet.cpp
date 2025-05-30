#include "IRInstrSet.h"

void IRInstrSet::genASM(ostream &o)
{
#ifdef __APPLE__
    o << ".globl _" << this->bb->getCFG()->getLabel() << "\n";
    o << " _" << this->bb->getCFG()->getLabel() << ":\n";
#else
    o << ".globl " << this->bb->getCFG()->getLabel() << "\n";
    o << this->bb->getCFG()->getLabel() << ":\n";
#endif

    o << "   pushq %rbp\n";
    o << "   movq %rsp, %rbp\n";
    o << "   subq $16, %rsp\n ";
}
