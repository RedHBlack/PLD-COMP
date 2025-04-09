#pragma once

#include "BaseIRInstr.h"

/**
 * @brief Represents a unary operation instruction in the intermediate representation.
 *
 * This class handles the generation of intermediate representation instructions for unary operations,
 * such as negation, logical negation, and bitwise complement.
 */
class IRInstrUnaryOp : public BaseIRInstr
{
public:
    /**
     * @brief Constructs a unary operation instruction.
     *
     * Initializes the instruction with a basic block, a unique operation identifier, and the operation itself.
     *
     * @param bb_ The basic block to which the instruction belongs.
     * @param uniqueOp A unique identifier for the operation.
     * @param op The actual unary operation (e.g., '!', '-', '~').
     */
    IRInstrUnaryOp(BasicBlock *bb_, string uniqueOp, string op)
        : BaseIRInstr(bb_), uniqueOp(uniqueOp), op(op) {}

    /**
     * @brief Generates the assembly code for this unary operation instruction.
     *
     * This method generates the appropriate assembly code based on the specific unary operation (e.g., negation,
     * logical NOT, bitwise complement).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void gen_asm(ostream &o);

protected:
    /// A unique identifier for the unary operation.
    string uniqueOp;

    /// The actual unary operation (e.g., '!', '-', '~').
    string op;

private:
    /**
     * @brief Handles the logical NOT operation.
     *
     * This method generates assembly code for the logical NOT operation.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleNot(ostream &o);

    /**
     * @brief Handles the negation operation.
     *
     * This method generates assembly code for the negation operation (e.g., `-`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleNeg(ostream &o);

    /**
     * @brief Handles the bitwise complement operation.
     *
     * This method generates assembly code for the bitwise complement operation (e.g., `~`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleCompl(ostream &o);
};
