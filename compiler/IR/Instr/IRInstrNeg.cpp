#include "IRInstrNeg.h"

void IRInstrNeg::gen_asm(ostream &o)
{
    o << "   negl %eax\n";
}