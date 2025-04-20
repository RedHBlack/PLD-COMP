#pragma once

#include "IRInstrBinaryOp.h"

/**
 * @brief A class representing a logical operation instruction.
 *
 * This class represents an instruction for the lazy evaluation version of the logical AND (`&&`) and OR (`||`)
 */
class IRInstrLogical : public IRInstrBinaryOp
{
public:
    /**
     * @brief Constructs a logical operation instruction.
     *
     * Initializes the instruction with a basic block, two operands, and the logical operation.
     *
     * @param bb_ The basic block to which the instruction belongs.
     * @param firstOp The first operand of the logical operation.
     * @param secondOp The second operand of the logical operation.
     * @param op The logical operation (e.g., '&&', '||').
     */
    IRInstrLogical(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    /**
     * @brief Generates the assembly code for this logical operation instruction.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void genASM(ostream &o) override;

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