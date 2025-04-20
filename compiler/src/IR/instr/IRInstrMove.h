#pragma once

#include <string>
#include <ostream>
#include "../BasicBlock.h"
#include "IRInstrBinaryOp.h"

using namespace std;

/**
 * @class IRInstrMove
 * @brief Represents an IR instruction for moving a value between registers and memory.
 *
 * This class handles data transfers between registers and the stack but does not manage constants.
 */
class IRInstrMove : public BaseIRInstr
{
public:
    /**
     * @brief Constructor for the IRInstrMove instruction.
     *
     * Initializes the instruction with the basic block, source, and destination variables.
     *
     * @param bb_ Pointer to the basic block containing this instruction.
     * @param src The name of the source variable (register or memory location).
     * @param dest The name of the destination variable (register or memory location).
     */
    IRInstrMove(BasicBlock *bb_, string src, string dest);

    /**
     * @brief Generates the assembly code corresponding to the move instruction.
     *
     * This method generates the appropriate assembly code for moving a value from the source variable to
     * the destination variable.
     *
     * @param o Output stream where the assembly code will be written.
     */
    virtual void genASM(ostream &o) override;

private:
    // The source variable (register or memory location).
    string src;

    // The destination variable (register or memory location).
    string dest;
};
