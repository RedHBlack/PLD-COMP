#include "IRInstrDiv.h"

void IRInstrDiv::gen_asm(ostream &o)
{
    o << "   cdq\n";
    o << "   idivl " << src << "\n";
}
