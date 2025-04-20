#include "IRInstrJmpCond.h"

void IRInstrJmpCond::genASM(ostream &o)
{
    o << "   cmpl $0, %" << reg << endl;
    o << "   " << condition << " " << label << endl;
}