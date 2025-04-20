#pragma once

#include "BaseIRInstr.h"
#include <ostream>

class IRInstrJmpRet : public BaseIRInstr
{
public:
	/**
	 * @brief Constructs an IRInstrJmpRet object.
	 *
	 * @param bb_ The basic block to which this instruction belongs.
	 * @param label The label of the basic block to jump to after returning.
	 */
	IRInstrJmpRet(BasicBlock *bb_, string label)
		: BaseIRInstr(bb_), label(label) {}
	/**
	 * @brief Generates the assembly code for ret instruction.
	 *
	 * @param o The output stream where the generated assembly code will be written.
	 */
	virtual void gen_asm(ostream &o) override;

private:
	/// The label of the basic block to jump to after returning.
	string label;
};