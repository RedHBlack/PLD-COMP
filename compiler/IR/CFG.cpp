#include "CFG.h"
#include "Instr/IRInstrSet.h"
#include <sstream>
#include "Instr/IRInstrJmpCond.h"
#include "Instr/IRInstrJmpRet.h"

CFG::CFG(string label, map<string, int> SymbolIndex, map<string, Type> SymbolType, int initialNextFreeSymbolIndex, int idBB) : label(label), SymbolIndex(SymbolIndex), SymbolType(SymbolType), nextFreeSymbolIndex(initialNextFreeSymbolIndex), initialTempPos(initialNextFreeSymbolIndex), idBB(0)
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
        o << bbs[i]->getLabel() << ":" << endl;
        bbs[i]->gen_asm(o);
    }
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

string CFG::getBBName()
{
    return "BB_" + to_string(idBB++);
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

void CFG::setLabel(string label)
{
    this->label = label;
}

void CFG::add_if_then_else(BasicBlock *test, BasicBlock *then_bb, BasicBlock *else_bb, BasicBlock *end_bb)
{
    // Ajouter les sauts conditionnels depuis le bloc de test
    test->add_IRInstr(new IRInstrJmpCond(test, "je", else_bb->getLabel(), "edx"));

    // À la fin du bloc then, sauter à end_bb
    then_bb->add_IRInstr(new IRInstrJmpRet(then_bb, end_bb->getLabel()));

    // Pas besoin de jump à la fin du else car le flot de contrôle continue naturellement
}

void CFG::add_while(BasicBlock *test, BasicBlock *body, BasicBlock *end_bb)
{
    // Le test saute à end_bb si la condition est fausse
    test->add_IRInstr(new IRInstrJmpCond(test, "je", end_bb->getLabel(), "edx"));

    // À la fin du corps, retourner au test
    body->add_IRInstr(new IRInstrJmpRet(body, test->getLabel()));
}