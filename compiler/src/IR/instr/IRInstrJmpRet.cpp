#include "IRInstrJmpRet.h"

void IRInstrJmpRet::genASM(ostream &o)
{
	o << "   jmp " << label << endl;
}