#pragma once

#include "BaseIRInstr.h"
#include <string>
using namespace std;

class IRInstrStoreToArray : public BaseIRInstr
{
public:
    // Pour un index dynamique, 'indexRegister' sera non vide (par exemple "%rax").
    // Pour un index constant, on passe une chaîne vide et 'baseOffset' contiendra l'offset complet.
    IRInstrStoreToArray(BasicBlock *bb, int baseOffset, string indexRegister, string sourceRegister);
    virtual void gen_asm(ostream &o);

private:
    int baseOffset;
    string indexRegister;
    string sourceRegister;
};
