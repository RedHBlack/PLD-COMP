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
class IRInstrMove : public IRInstrBinaryOp
{
public:
    /**
     * @brief Constructor for the IRInstrMove instruction.
     * @param bb_ Pointer to the basic block containing this instruction.
     * @param src The name of the source variable (register or memory location).
     * @param dest The name of the destination variable (register or memory location).
     */
    IRInstrMove(BasicBlock *bb_, string src, string dest);

    /**
     * @brief Generates the assembly code corresponding to the move instruction.
     * @param o Output stream where the assembly code is written.
     */
    virtual void gen_asm(ostream &o) override;

private:
};
