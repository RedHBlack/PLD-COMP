#include "CFG.h"

CFG::CFG(tree::ParseTree *ast)
{
}

void CFG::add_bb(BasicBlock *bb)
{
}

void CFG::gen_asm(ostream &o)
{
}

string CFG::IR_reg_to_asm(string reg)
{
    return string();
}

void CFG::gen_asm_prologue(ostream &o)
{
}

void CFG::gen_asm_epilogue(ostream &o)
{
}

void CFG::add_to_symbol_table(string name, Type t)
{
}

string CFG::create_new_tempvar(Type t)
{
    return string();
}

int CFG::get_var_index(string name)
{
    return 0;
}

Type CFG::get_var_type(string name)
{
    return Type();
}

string CFG::new_BB_name()
{
    return string();
}
