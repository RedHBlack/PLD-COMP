#pragma once

#include "BaseIRInstr.h"

/**
 * @brief Represents a binary operation instruction in the intermediate representation.
 *
 * This class serves as the base class for binary operation instructions such as addition, subtraction, multiplication, etc.
 * It provides a structure for managing two operands and the operation itself, and it also handles the generation
 * of the corresponding assembly code for binary operations.
 */
class IRInstrBinaryOp : public BaseIRInstr
{
public:
    /**
     * @brief Constructs a binary operation instruction.
     *
     * Initializes the instruction with a basic block, two operands, and the binary operation.
     *
     * @param bb_ The basic block to which the instruction belongs.
     * @param firstOp The first operand of the binary operation.
     * @param secondOp The second operand of the binary operation.
     * @param op The binary operation (e.g., '+', '-', '*', '/').
     */
    IRInstrBinaryOp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : BaseIRInstr(bb_), firstOp(firstOp), secondOp(secondOp), op(op) {}

    /**
     * @brief Generates the assembly code for this binary operation instruction.
     *
     * This method must be implemented by derived classes to generate the appropriate assembly code based on the
     * specific binary operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void gen_asm(ostream &o) = 0;

protected:
    /// The first operand for the binary operation.
    string firstOp;

    /// The second operand for the binary operation.
    string secondOp;

    /// The binary operation (e.g., '+', '-', '*', '/','%').
    string op;
};
