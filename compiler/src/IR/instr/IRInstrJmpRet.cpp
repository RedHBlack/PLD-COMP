#include "IRInstrJmpRet.h"

void IRInstrJmpRet::gen_asm(ostream &o)
{
	o << "   jmp " << label << endl;
}