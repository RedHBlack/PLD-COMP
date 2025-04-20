#pragma once

#include "BaseIRInstr.h"
#include <string>
using namespace std;

/**
 * @brief A class representing an instruction that loads an array element into a register.
 *
 * This class represents an instruction that loads an array element into a register.
 */
class IRInstrLoadFromArray : public BaseIRInstr
{
public:
    /**
     * @brief Construct a new IRInstrLoadFromArray object
     *
     * @param bb The basic block to which this instruction belongs.
     * @param src The source array name.
     * @param dest The destination register.
     */
    IRInstrLoadFromArray(BasicBlock *bb, string src, string dest, int index);

    /**
     * @brief Generate assembly code for loading an array element into a register.
     *
     * @param o The output stream to write the generated assembly code.
     */
    virtual void gen_asm(ostream &o);

private:
    /// Source array name
    string src;
    /// Destination register
    string dest;
    /// Index of the array element to load
    int index;
};