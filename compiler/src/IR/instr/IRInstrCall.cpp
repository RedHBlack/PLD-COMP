#include "IRInstrCall.h"

void IRInstrCall::genASM(ostream &o)
{

    o << "   call " << funcName << "\n";
}