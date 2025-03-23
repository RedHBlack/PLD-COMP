#pragma once

#include "BaseIRInstr.h"
#include <string>

/**
 * @class IRInstrLoadConst
 * @brief Represents an IR instruction for loading a constant into memory or a register.
 *
 * This instruction is used to assign an immediate value to a register or memory location.
 */
class IRInstrLoadConst : public BaseIRInstr
{
public:
    /**
     * @brief Constructor for the IRInstrLoadConst instruction.
     * @param bb_ Pointer to the basic block containing this instruction.
     * @param value The constant value to load.
     * @param dest The name of the target register or memory variable.
     */
    IRInstrLoadConst(BasicBlock *bb_, int value, string dest);

    /**
     * @brief Generates assembly code to load a constant.
     * @param o Output stream where the assembly code is written.
     */
    virtual void gen_asm(ostream &o) override;

private:
    int value;   /**< The constant value to load. */
    string dest; /**< Name of the destination variable. */
};
