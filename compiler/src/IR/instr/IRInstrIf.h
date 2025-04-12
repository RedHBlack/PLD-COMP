#pragma once

#include "BaseIRInstr.h"
#include <ostream>

class IRInstrIf : public BaseIRInstr
{
public:
	IRInstrIf(BasicBlock *bb_, string condReg, string label)
		: BaseIRInstr(bb_), condReg(condReg), label(label) {}

	virtual void gen_asm(ostream &o) override;
private:
	string condReg;
	string label;
};