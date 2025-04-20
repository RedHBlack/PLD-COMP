#pragma once

#include "BaseIRInstr.h"
#include <string>
using namespace std;

/**
 * @brief A class representing an instruction to store data at an array location.
 *
 */
class IRInstrStoreToArray : public BaseIRInstr
{
public:
    /**
     * @brief Construct a new IRInstrStoreToArray object.
     *
     * @param bb The basic block to which this instruction belongs.
     * @param baseOffset The offset from the start of the array.
     * @param indexRegister The register holding the index of the array element to store.
     * @param sourceRegister The register holding the value to be stored.
     */
    IRInstrStoreToArray(BasicBlock *bb, int baseOffset, string indexRegister, string sourceRegister);

    /**
     * @brief Generate assembly code for storing data at an array location.
     *
     * @param o The output stream to write the generated assembly code.
     */
    virtual void genASM(ostream &o);

private:
    /// The offset from the start of the array.
    int baseOffset;

    /// The register holding the index of the array element to store.
    string indexRegister;

    /// The register holding the value to be stored.
    string sourceRegister;
};
