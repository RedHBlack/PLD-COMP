
#include "BaseIRInstr.h"

class IRInstrLDConst : public BaseIRInstr
{

public:
    IRInstrLDConst(BasicBlock *bb_, Operation op, Type t, vector<string> params);

private:
};