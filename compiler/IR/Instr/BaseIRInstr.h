#pragma once

#include "../BasicBlock.h"
#include "../../SymbolsTable.h"
#include <string>
#include <ostream>

using namespace std;

class BasicBlock;

/**
 * @brief Represents a base class for intermediate representation instructions.
 *
 * This class serves as the base for all instruction types in the intermediate representation (IR). It provides
 * a basic structure for handling assembly generation and access to the basic block that the instruction belongs to.
 */
class BaseIRInstr
{
public:
    /**
     * @brief Constructs an instruction for a given basic block.
     *
     * Initializes the instruction with the basic block it belongs to.
     *
     * @param bb_ The basic block to which this instruction belongs.
     */
    BaseIRInstr(BasicBlock *bb_);

    /**
     * @brief Gets the basic block that this instruction belongs to.
     *
     * @return The basic block associated with this instruction.
     */
    BasicBlock *getBB();

    /**
     * @brief Generates the assembly code for this instruction.
     *
     * This is a pure virtual function that must be implemented by derived classes to generate the specific
     * assembly code for each type of instruction.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    virtual void gen_asm(ostream &o) = 0;

protected:
    /// The basic block that this instruction belongs to.
    BasicBlock *bb;
    SymbolsTable *symbolsTable;
};
