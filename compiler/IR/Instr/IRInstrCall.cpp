#include "IRInstrCall.h"

void IRInstrCall::gen_asm(ostream &o)
{

    o << "   call " << funcName << "\n";
}