#pragma once
#include "BasicBlock.h"
#include <ostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class BasicBlock;

class CFG
{
public:
    CFG(string label, map<string, int> SymbolIndex, int initialNextFreeSymbolIndex);

    void add_bb(BasicBlock *bb);

    // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
    void gen_asm(ostream &o);
    void gen_asm_prologue(ostream &o);
    void gen_asm_epilogue(ostream &o);

    // symbol table methods
    string create_new_tempvar();
    int get_var_index(string name);

    // basic block management
    BasicBlock *getCurrentBasicBlock();
    void setCurrentBasicBlock(BasicBlock *bb);

    void
    resetNextFreeSymbolIndex();

protected:
    map<string, int> SymbolIndex; /**< part of the symbol table  */
    int nextFreeSymbolIndex;      /**< to allocate new symbols in the symbol table */
    BasicBlock *current_bb;
    string label;

    const int initialTempPos;

    vector<BasicBlock *> bbs; /**< all the basic blocks of this CFG*/
};