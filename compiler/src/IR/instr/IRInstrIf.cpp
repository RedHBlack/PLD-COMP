#include "IRInstrIf.h"

void IRInstrIf::genASM(ostream &o)
{
    o << "    cmp $0, " << condReg << endl;
    o << "    je " << label << endl;
}