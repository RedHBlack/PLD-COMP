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
    const int offsetSrcVar = this->getBB()->getCFG()->get_var_index(this->src);
    const int offsetDestinationVar = this->getBB()->getCFG()->get_var_index(this->dest);

    if (src[0] != '%' && dest[0] != '%')
    {
        o << "   movl " << offsetSrcVar << "(%rbp), %eax\n";
        o << "   movl %eax, " << offsetDestinationVar << "(%rbp)\n";
    }
    else if (dest[0] != '%')
    {
        o << "   movl " << src << ", " << offsetDestinationVar << "(%rbp)\n";
    }
    else if (src[0] != '%')
    {
        o << "   movl " << offsetSrcVar << "(%rbp), " << dest << "\n";
    }
    else
    {
        o << "   movl " << src << ", " << dest << "\n";
    }
}
