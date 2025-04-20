#include "IRInstrClean.h"

void IRInstrClean::genASM(ostream &o)
{
    o << "   leave\n ";
    o << "   ret\n";
}