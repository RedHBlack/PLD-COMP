#include "IRInstrIf.h"

void IRInstrIf::gen_asm(ostream &o)
{
	o << "    cmp $0, " << condReg << endl;
    o << "    je " << label << endl;	
}