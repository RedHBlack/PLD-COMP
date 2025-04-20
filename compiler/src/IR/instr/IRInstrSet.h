#pragma once

#include "BaseIRInstr.h"
#include <ostream>

/**
 * @class IRInstrSet
 * @brief Represents an instruction that sets a value in the intermediate representation.
 *
 * The `IRInstrSet` class is a subclass of `BaseIRInstr`. It represents an instruction that
 * performs an assignment or setting of a value within a basic block during intermediate representation
 * generation. The primary function of this class is to generate the assembly code for the instruction.
 */
class IRInstrSet : public BaseIRInstr
{
public:
    /**
     * @brief Constructs an IRInstrSet object.
     *
     * This constructor initializes an `IRInstrSet` instance with a reference to the basic block
     * in which this instruction resides.
     *
     * @param bb_ A pointer to the `BasicBlock` to which this instruction belongs.
     */
    IRInstrSet(BasicBlock *bb_) : BaseIRInstr(bb_) {};

    /**
     * @brief Generates assembly code for the IRInstrSet instruction.
     *
     * This function generates the corresponding assembly code for the `IRInstrSet` instruction
     * and writes it to the provided output stream.
     *
     * The generated assembly code typically includes an instruction for setting a value in a register
     * or memory location, depending on the context.
     *
     * @param o The output stream to which the generated assembly code will be written.
     */
    virtual void genASM(std::ostream &o) override;
};
