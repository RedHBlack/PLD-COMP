#pragma once

#include "BaseIRInstr.h"
#include <string>
using namespace std;

class IRInstrLoadFromArray : public BaseIRInstr
{
public:
    // Si 'index' est négatif, cela signifie que l'index a été évalué dynamiquement et se trouve dans %rax.
    // Sinon, 'index' contient la valeur constante.
    IRInstrLoadFromArray(BasicBlock *bb, string src, string dest, int index);
    virtual void gen_asm(ostream &o);

private:
    string src;  // Nom du tableau
    string dest; // Registre cible
    int index;
};