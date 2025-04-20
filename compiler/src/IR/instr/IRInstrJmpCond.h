#pragma once

#include "BaseIRInstr.h"

/**
 * @brief A class representing a conditional jump instruction in the IR.
 *
 * This class extends the BaseIRInstr class and represents a conditional jump instruction.
 */
class IRInstrJmpCond : public BaseIRInstr
{
public:
    /**
     * @brief Construct a new IRInstrJmpCond object.
     *
     * @param bb The basic block to which this instruction belongs.
     * @param condition The condition under which the jump occurs.
     * @param label The destination label for the jump.
     * @param reg The register to use for testing the condition.
     */
    IRInstrJmpCond(BasicBlock *bb, string condition, string label, string reg)
        : BaseIRInstr(bb), condition(condition), label(label), reg(reg) {}

    /**
     * @brief Generate assembly code for the conditional jump instruction.
     *
     * @param o The output stream to write the generated assembly code.
     */
    virtual void gen_asm(ostream &o) override;

private:
    /// The condition under which the jump occurs .
    string condition;
    /// The destination label for the jump .
    string label;
    /// The register to use for testing the condition .
    string reg;
};