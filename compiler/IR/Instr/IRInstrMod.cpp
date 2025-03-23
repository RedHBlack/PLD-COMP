#include "IRInstrMod.h"

void IRInstrMod::gen_asm(ostream &o)
{
    o << "   cdq\n";
    o << "   idivl " << src << "\n";
    o << "   movl %edx" << ", " << dest << "\n";
}
