#pragma once

#include "BaseIRInstr.h"
#include <vector>
#include <string>

/**
 * @brief A class representing an intermediate representation instruction for a function call.
 *
 * This class extends the BaseIRInstr class and represents an instruction for making a function call.
 * It includes information about the basic block it belongs to and the name of the function being called.
 *
 */
class IRInstrCall : public BaseIRInstr
{
public:
    /**
     * @brief Constructs an IRInstrCall object.
     *
     * @param bb The basic block to which this instruction belongs.
     * @param funcName The name of the function being called.
     */
    IRInstrCall(BasicBlock *bb, const string funcName)
        : BaseIRInstr(bb), funcName(funcName) {}

    /**
     * @brief Generates the assembly code for this function call instruction.
     *
     * @param o The output stream where the generated assembly code will be written.
     */
    void gen_asm(ostream &o) override;

private:
    //// The name of the function being called.
    string funcName;
};
