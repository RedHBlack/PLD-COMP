#include "IRInstrAdd.h"

void IRInstrAdd::gen_asm(ostream &o)
{
    o << "   addl " << src << ", " << dest << "\n";
}
