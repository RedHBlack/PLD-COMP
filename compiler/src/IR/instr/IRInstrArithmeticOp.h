#pragma once

#include "BaseIRInstr.h"
#include "IRInstrBinaryOp.h"

/**
 * @brief Represents an arithmetic operation instruction in the intermediate representation.
 *
 * This class handles the generation of intermediate representation instructions for arithmetic operations,
 * such as addition, subtraction, multiplication, division, modulo, and bitwise operations. It extends the
 * `IRInstrBinaryOp` class and provides specialized methods for handling these operations.
 */
class IRInstrArithmeticOp : public IRInstrBinaryOp
{
public:
    /**
     * @brief Constructs an arithmetic operation instruction.
     *
     * Initializes the instruction with a basic block, two operands, and the arithmetic operation.
     *
     * @param bb_ The basic block to which the instruction belongs.
     * @param firstOp The first operand of the arithmetic operation.
     * @param secondOp The second operand of the arithmetic operation.
     * @param op The arithmetic operation (e.g., '+', '-', '*', '/', '%').
     */
    IRInstrArithmeticOp(BasicBlock *bb_, string firstOp, string secondOp, string op)
        : IRInstrBinaryOp(bb_, firstOp, secondOp, op) {}

    /**
     * @brief Generates the assembly code for this arithmetic operation instruction.
     *
     * This method generates the appropriate assembly code based on the specific arithmetic operation
     * (e.g., addition, subtraction, multiplication, division, modulo, or bitwise operations).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void gen_asm(ostream &o);

private:
    /**
     * @brief Handles the addition operation.
     *
     * This method generates assembly code for the addition operation (`+`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleAddition(ostream &o);

    /**
     * @brief Handles the subtraction operation.
     *
     * This method generates assembly code for the subtraction operation (`-`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleSubstraction(ostream &o);

    /**
     * @brief Handles the multiplication operation.
     *
     * This method generates assembly code for the multiplication operation (`*`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleMult(ostream &o);

    /**
     * @brief Handles the division operation.
     *
     * This method generates assembly code for the division operation (`/`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleDiv(ostream &o);

    /**
     * @brief Handles the modulo operation.
     *
     * This method generates assembly code for the modulo operation (`%`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleModulo(ostream &o);

    /**
     * @brief Handles the bitwise AND operation.
     *
     * This method generates assembly code for the bitwise AND operation (`&`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleBitwiseAnd(ostream &o);

    /**
     * @brief Handles the bitwise OR operation.
     *
     * This method generates assembly code for the bitwise OR operation (`|`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleBitwiseOr(ostream &o);

    /**
     * @brief Handles the bitwise XOR operation.
     *
     * This method generates assembly code for the bitwise XOR operation (`^`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleBitwiseXor(ostream &o);

    /**
     * @brief Handles the left shift operation.
     *
     * This method generates assembly code for the left shift operation (`<<`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleLeftShift(ostream &o);

    /**
     * @brief Handles the right shift operation.
     *
     * This method generates assembly code for the right shift operation (`>>`).
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void handleRightShift(ostream &o);
};
