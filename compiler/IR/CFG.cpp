#include "CFG.h"
#include "Instr/IRInstrSet.h"
#include <sstream>

CFG::CFG(string label, map<string, int> SymbolIndex, map<string, Type> SymbolType, int initialNextFreeSymbolIndex) : label(label), SymbolIndex(SymbolIndex), SymbolType(SymbolType), nextFreeSymbolIndex(initialNextFreeSymbolIndex), initialTempPos(initialNextFreeSymbolIndex)
{
    BasicBlock *input = new BasicBlock(this, "input");
    input->add_IRInstr(new IRInstrSet(input));

    bbs.push_back(input);
}

void CFG::add_bb(BasicBlock *bb)
{
    if (bbs.size() == 1)
        bbs[0]->setExitTrue(bb);

    bbs.push_back(bb);
    this->setCurrentBasicBlock(bb);
}

void CFG::gen_asm(ostream &o)
{

    for (int i = 0; i < bbs.size(); i++)
    {
        bbs[i]->gen_asm(o);
    }

    // mettre ici le code pour les cas de if/else-if/else. L'idée c'est de les mettre à la fin du code généré
}

string CFG::create_new_tempvar(Type t)
{
    const string newTmpVar = "tmp" + to_string(-nextFreeSymbolIndex);

    this->SymbolIndex[newTmpVar] = nextFreeSymbolIndex;
    this->nextFreeSymbolIndex -= 4;

    this->SymbolType[newTmpVar] = t;

    return newTmpVar;
}

int CFG::get_var_index(string name)
{
    return this->SymbolIndex[name];
}

Type CFG::get_var_type(string name)
{
    return this->SymbolType[name];
}

BasicBlock *CFG::getCurrentBasicBlock()
{
    return this->current_bb;
}

void CFG::setCurrentBasicBlock(BasicBlock *bb)
{
    this->current_bb = bb;
}

void CFG::resetNextFreeSymbolIndex()
{
    nextFreeSymbolIndex = initialTempPos;
}

void CFG::gen_cfg_graphviz(ostream &o)
{
    o << "digraph CFG {\n";
    o << "    node [shape=record, style=filled, fillcolor=lightgray];\n";

    for (BasicBlock *bb : bbs)
    {

        stringstream label;

        label << "{ " + bb->getLabel() + " | ";

        for (BaseIRInstr *instr : bb->getInstr())
        {
            stringstream temp;
            instr->gen_asm(temp);

            string instrStr = temp.str();

            int pos = 0;
            while ((pos = instrStr.find('\n', pos)) != string::npos)
            {
                instrStr.replace(pos, 1, " \\l");
                pos += 4;
            }

            label << instrStr;
        }
        label << "}";

        o << "    " << bb->getLabel() << " [label=\"" << label.str() << "\"];\n";

        if (bb->getExitTrue())
            o << "  \"" << bb->getLabel() << "\" -> \"" << bb->getExitTrue()->getLabel() << "\";\n";
        else if (bb->getExitFalse())
            o << "  \"" << bb->getLabel() << "\" -> \"" << bb->getExitFalse()->getLabel() << "\";\n";
    }

    o << "}\n";
}

string CFG::getLabel()
{
    return label;
}
