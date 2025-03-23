#include "IRInstrAnd.h"

void IRInstrAnd::gen_asm(ostream &o)
{
    o << "   andl " << src << ", " << dest << "\n";
}
