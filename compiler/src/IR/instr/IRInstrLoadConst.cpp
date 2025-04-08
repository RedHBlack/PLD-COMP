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

    o << "   movl $" << value << " ," << dest << "\n";
}
