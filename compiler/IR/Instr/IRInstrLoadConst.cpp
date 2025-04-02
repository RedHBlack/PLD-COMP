#include "IRInstrLoadConst.h"
#include <iostream>
#include <cstring>

/**
 * @brief Constructor for the IRInstrLoadConst instruction.
 */
IRInstrLoadConst::IRInstrLoadConst(BasicBlock *bb_, int value, std::string dest)
    : BaseIRInstr(bb_), value(value), dest(dest) {}

/**
 * @brief Generates assembly code to load a constant into memory or a register.
 */
void IRInstrLoadConst::gen_asm(std::ostream &o)
{
    // If the destination is a register
    if (dest[0] == '%' || dest[0] == '-')
    {
        // If the destination is a register, we can directly load the value into it
        o << "   movl $" << value << ", " << dest << "\n";
    }
    else
    {
        const int offsetDestinationVar = this->getBB()->getCFG()->get_var_index(this->dest);
        o << "   movl $" << value << " ," << offsetDestinationVar << "(%rbp)\n";
    }
}
