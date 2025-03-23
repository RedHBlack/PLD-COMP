#include "IRInstrMult.h"

void IRInstrMult::gen_asm(ostream &o)
{
    o << "   imull " << src << ", " << dest << "\n";
}
