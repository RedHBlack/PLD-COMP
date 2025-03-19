
#include "BaseIRInstr.h"

class IRInstrAdd : public BaseIRInstr
{

public:
    IRInstrAdd(BasicBlock *bb_, Operation op, Type t, vector<string> params);

private:
};