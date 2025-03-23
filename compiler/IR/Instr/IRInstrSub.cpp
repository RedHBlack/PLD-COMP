#include "IRInstrSub.h"

void IRInstrSub::gen_asm(ostream &o)
{
    o << "   subl " << src << ", " << dest << "\n";
}
