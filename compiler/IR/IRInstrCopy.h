
#include "BaseIRInstr.h"

class IRInstrCopy : public BaseIRInstr
{

public:
    IRInstrCopy(BasicBlock *bb_, Operation op, Type t, vector<string> params);

private:
};