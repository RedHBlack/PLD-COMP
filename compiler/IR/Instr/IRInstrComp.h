#pragma once

#include "IRInstrBinaryOp.h"
#include <ostream>

/**
 * @brief Represents a comparison operation instruction in the intermediate representation.
 *
 * This class handles the generation of intermediate representation instructions for comparison operations,
 * such as equality, inequality, greater than, greater than or equal to, less than, and less than or equal to.
 */
class IRInstrComp : public IRInstrBinaryOp
{
public:
    /**
     * @brief Constructs a comparison operation instruction.
     *
     * Initializes the instruction with a basic block, two operands, and the comparison operation.
     *
     * @param bb_ The basic block to which the instruction belongs.
     * @param firstOp The first operand of the comparison operation.
     * @param secondOp The second operand of the comparison operation.
     * @param op The comparison operation (e.g., '==', '!=', '>', '<', '>=', '<=').
     */
    IRInstrComp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    /**
     * @brief Generates the assembly code for this comparison operation instruction.
     *
     * This method generates the appropriate assembly code based on the specific comparison operation
     * (e.g., equality, inequality, greater than, greater than or equal to, less than, or less than or equal to).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void gen_asm(ostream &o) override;

private:
    /**
     * @brief Handles the equality comparison operation (`==`).
     *
     * This method generates assembly code for the equality comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleCompEq(ostream &o);

    /**
     * @brief Handles the inequality comparison operation (`!=`).
     *
     * This method generates assembly code for the inequality comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleCompNotEq(ostream &o);

    /**
     * @brief Handles the greater-than comparison operation (`>`).
     *
     * This method generates assembly code for the greater-than comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleGreater(ostream &o);

    /**
     * @brief Handles the greater-than or equal-to comparison operation (`>=`).
     *
     * This method generates assembly code for the greater-than or equal-to comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleGreaterOrEqual(ostream &o);

    /**
     * @brief Handles the less-than comparison operation (`<`).
     *
     * This method generates assembly code for the less-than comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleLower(ostream &o);

    /**
     * @brief Handles the less-than or equal-to comparison operation (`<=`).
     *
     * This method generates assembly code for the less-than or equal-to comparison operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleLowerOrEqual(ostream &o);
};
