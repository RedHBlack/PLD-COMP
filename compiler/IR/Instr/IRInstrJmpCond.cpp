#include "IRInstrJmpCond.h"

void IRInstrJmpCond::gen_asm(ostream &o)
{
    o << "   cmpl $0, %" << reg << endl;
    o << "   " << condition << " " << label << endl;
}