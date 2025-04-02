#include "BaseIRInstr.h"
#include <ostream>

class IRInstrJump : public BaseIRInstr {
public:
    IRInstrJump(BasicBlock* bb, string label) : BaseIRInstr(bb), label(label) {}
    void gen_asm(ostream &o) override {
        o << "    jmp " << label << endl;
    }
private:
    string label;
};