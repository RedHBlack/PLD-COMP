#include "IRInstrXor.h"

void IRInstrXor::gen_asm(std::ostream &o)
{
    o << "   xorl " << src << ", " << dest << "\n";
}