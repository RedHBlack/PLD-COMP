#pragma once

#include "IRInstrBinaryOp.h"
#include <ostream>

class IRInstrIf : public IRInstrBinaryOp
{
public:
	IRInstrIf(BasicBlock *bb_, string firstOp, string secondOp, string op, string label)
		: IRInstrBinaryOp(bb_, firstOp, secondOp, op), label(label) {}

	virtual void gen_asm(ostream &o) override;
private:
	string label;
};