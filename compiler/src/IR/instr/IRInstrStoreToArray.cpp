#include "IRInstrStoreToArray.h"

IRInstrStoreToArray::IRInstrStoreToArray(BasicBlock *bb, int baseOffset, string indexRegister, string sourceRegister)
    : BaseIRInstr(bb), baseOffset(baseOffset), indexRegister(indexRegister), sourceRegister(sourceRegister)
{
}

void IRInstrStoreToArray::genASM(ostream &o)
{
    if (indexRegister == "")
    {
        // Cas d'un index constant : l'offset est déjà complet.
        o << "   movl " << sourceRegister << ", " << baseOffset << "(%rbp)\n";
    }
    else
    {
        // Cas d'un index dynamique.
        o << "   movl " << sourceRegister << ", " << baseOffset << "(%rbp," << indexRegister << ",4)\n";
    }
}
