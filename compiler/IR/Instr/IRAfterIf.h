#pragma once

#include "BaseIRInstr.h"
#include <ostream>

class IRAfterIf : public BaseIRInstr
{
public:
	IRAfterIf(BasicBlock *bb, string label) : BaseIRInstr(bb), label(label) {}
	
	void gen_asm(ostream &o);
private:
	string label;
};