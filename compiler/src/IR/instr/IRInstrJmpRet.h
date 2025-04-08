#pragma once

#include "BaseIRInstr.h"
#include <ostream>

class IRInstrJmpRet : public BaseIRInstr
{
public:
	IRInstrJmpRet(BasicBlock *bb_, string label)
		: BaseIRInstr(bb_), label(label) {}

	virtual void gen_asm(ostream &o) override;
private:
	string label;
};