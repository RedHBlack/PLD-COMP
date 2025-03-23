#pragma once

#include "CFG.h"
#include "Instr/BaseIRInstr.h"
#include <vector>

using namespace std;

class CFG;
class BaseIRInstr;

class BasicBlock
{
public:
  BasicBlock(CFG *cfg, string entry_label);
  void gen_asm(ostream &o); /**< x86 assembly code generation for this basic block (very simple) */
  void add_IRInstr(BaseIRInstr *instr);
  CFG *getCFG();

protected:
  string label;                 /**< label of the BB, also will be the label in the generated code */
  CFG *cfg;                     /** < the CFG where this block belongs */
  vector<BaseIRInstr *> instrs; /** < the instructions themselves. */
};