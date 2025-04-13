#pragma once

#include "BaseIRInstr.h"
#include <ostream>

/**
 * @class IRInstrClean
 * @brief Represents a clean-up instruction in the intermediate representation.
 *
 * The `IRInstrClean` class is a subclass of `BaseIRInstr`. It represents a clean-up instruction,
 * typically used to deallocate resources or reset values within a basic block during intermediate
 * representation generation. The main responsibility of this class is to generate the corresponding
 * assembly code for the clean-up operation.
 */
class IRInstrClean : public BaseIRInstr
{
public:
    /**
     * @brief Constructs an IRInstrClean object.
     *
     * This constructor initializes an `IRInstrClean` instance with a reference to the basic block
     * where this clean-up instruction resides.
     *
     * @param bb_ A pointer to the `BasicBlock` to which this instruction belongs.
     */
    IRInstrClean(BasicBlock *bb_) : BaseIRInstr(bb_) {};

    /**
     * @brief Generates assembly code for the IRInstrClean instruction.
     *
     * This function generates the assembly code corresponding to the clean-up operation represented
     * by this instruction and writes it to the provided output stream.
     *
     * The generated assembly code typically involves operations to reset, deallocate, or clean up
     * resources associated with the instruction.
     *
     * @param o The output stream to which the generated assembly code will be written.
     */
    virtual void gen_asm(std::ostream &o) override;
};
