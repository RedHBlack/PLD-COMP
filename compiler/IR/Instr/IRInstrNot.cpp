#include "IRInstrNot.h"

void IRInstrNot::gen_asm(ostream &o)
{
    o << "   testl " << src << ", " << src << "\n";
    o << "   movl $0, " << src << "\n";
    o << "   sete %al\n";
    o << "   movzbl %al, " << src << "\n";
}