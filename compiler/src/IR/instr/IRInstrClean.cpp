#include "IRInstrClean.h"

void IRInstrClean::gen_asm(ostream &o)
{
    o << "   leave\n ";
    o << "   ret\n";
}