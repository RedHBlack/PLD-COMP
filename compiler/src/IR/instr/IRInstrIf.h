#pragma once

#include "BaseIRInstr.h"
#include <ostream>

/**
 * @brief A class representing an if instruction in the intermediate representation.
 *
 * This class handles the creation and generation of assembly code for if statements in the compiler.
 *
 * @param bb_ Pointer to the BasicBlock object associated with this instruction.
 * @param condReg String containing the name of the condition register used in the comparison.
 * @param label String containing the label
 */
class IRInstrIf : public BaseIRInstr
{
public:
	/**
	 * @brief Constructor for the IRInstrIf class.
	 *
	 * Creates an instance of the IRInstrIf class with the provided parameters.
	 *
	 * @param bb_ The basic block to which the instruction belongs.
	 * @param condReg Register used in the comparison.
	 * @param label Label of the next block
	 */
	IRInstrIf(BasicBlock *bb_, string condReg, string label)
		: BaseIRInstr(bb_), condReg(condReg), label(label) {}

	/**
	 * @brief Generate assembly code for the if instruction.
	 *
	 * This method generates the appropriate assembly code based on the condition register and label.
	 *
	 * @param o Output stream to write the assembly code.
	 */
	virtual void gen_asm(ostream &o) override;

private:
	/// Condition register used in the comparison.
	string condReg;

	/// Label of the next block
	string label;
};