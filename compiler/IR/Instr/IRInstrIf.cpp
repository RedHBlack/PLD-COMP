#include "IRInstrIf.h"

void IRInstrIf::gen_asm(ostream &o)
{
	o << "    cmp " << firstOp << ", " << secondOp << endl;
	o << "    j";

	if (op == "==")
	{
		o << "e ";
	}
	else if (op == "!=")
	{
		o << "ne ";
	}
	else if (op == ">")
	{
		o << "g ";
	}
	else if (op == "<")
	{
		o << "l ";
	}
	else if (op == ">=")
	{
		o << "ge ";
	}
	else if (op == "<=")
	{
		o << "le ";
	}else{
		o << "mp ";
	}

	o << label << endl;
	
}