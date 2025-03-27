#include "IRInstrMove.h"
#include <iostream>

/**
 * @brief Constructor for the IRInstrMove instruction.
 */
IRInstrMove::IRInstrMove(BasicBlock *bb_, string src, string dest)
    : BaseIRInstr(bb_), src(src), dest(dest)
{
}

/**
 * @brief Generates assembly code for moving a value between registers and memory.
 */
void IRInstrMove::gen_asm(ostream &o)
{

    if (src[0] != '%' && dest[0] != '%')
    {
        o << "   movl " << this->symbolsTable->getSymbolIndex(this->src) << "(%rbp), %eax\n";
        o << "   movl %eax, " << this->symbolsTable->getSymbolIndex(this->dest) << "(%rbp)\n";
    }
    else if (dest[0] != '%')
    {
        o << "   movl " << src << ", " << this->symbolsTable->getSymbolIndex(this->dest) << "(%rbp)\n";
    }
    else if (src[0] != '%')
    {
        o << "   movl " << this->symbolsTable->getSymbolIndex(this->src) << "(%rbp), " << dest << "\n";
    }
    else
    {
        o << "   movl " << src << ", " << dest << "\n";
    }
}
