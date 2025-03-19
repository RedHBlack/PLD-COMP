
#include "BaseIRInstr.h"

class IRInstrMul : public BaseIRInstr
{

public:
    IRInstrMul(BasicBlock *bb_, Operation op, Type t, vector<string> params);

private:
};