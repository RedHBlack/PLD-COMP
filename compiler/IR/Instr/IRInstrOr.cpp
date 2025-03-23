#include "IRInstrOr.h"

void IRInstrOr::gen_asm(ostream &o)
{
    o << "   orl " << src << ", " << dest << "\n";
}
