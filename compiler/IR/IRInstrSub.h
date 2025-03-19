#include "BaseIRInstr.h";

class IRInstrSub : public BaseIRInstr
{

public:
    IRInstrSub(BasicBlock *bb_, Operation op, Type t, vector<string> params);

private:
};