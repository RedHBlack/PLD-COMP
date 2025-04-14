#pragma once

#include "IRInstrBinaryOp.h"

class IRInstrLogical : public IRInstrBinaryOp
{
public:
    /**
     * @brief Constructor for the IRInstrLoadConst instruction.
     *
     * Initializes the instruction with the basic block, the constant value to load, and the destination
     * register or memory variable where the value should be stored.
     *
     * @param bb_ Pointer to the basic block containing this instruction.
     * @param firstOp The first operand of the logical operation.
     * @param secondOp The second operand of the logical operation.
     * @param op The logical operation to perform.
     */
    IRInstrLogical(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    /**
     * @brief Generates assembly code to load a constant.
     *
     * Generates assembly code for loading a constant value into a register or memory.
     *
     * @param o Output stream where the assembly code will be written.
     */
    virtual void gen_asm(ostream &o) override;

private:
    /**
     * @brief Handles the && logical operation.
     *
     * This method generates assembly code for the `&&` logical operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleLogicalAnd(ostream &o);

    /**
     * @brief Handles the || logical operation.
     *
     * This method generates assembly code for the `||` logical operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleLogicalOr(ostream &o);

    /**
     * @brief Label counter for the logical operations.
     */
    static int label;
};