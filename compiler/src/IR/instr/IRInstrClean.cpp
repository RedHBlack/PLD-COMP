#include "IRInstrClean.h"

void IRInstrClean::gen_asm(ostream &o)
{
    o << "   popq %rbp\n";
    o << "   ret\n";
}