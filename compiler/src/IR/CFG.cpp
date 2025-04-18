#include "CFG.h"
#include "../utils/Type.h"
#include "instr/IRInstrSet.h"
#include <sstream>
#include "instr/IRInstrJmpCond.h"
#include "instr/IRInstrJmpRet.h"

CFG::CFG(string label, SymbolsTable *symbolsTable, int initialNextFreeSymbolIndex) : label(label), symbolsTable(symbolsTable), nextFreeSymbolIndex(initialNextFreeSymbolIndex), initialTempPos(initialNextFreeSymbolIndex)
{
    idBB = 0;
}

void CFG::add_bb(BasicBlock *bb)
{
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

    this->symbolsTable->addSymbol(newTmpVar, t, size_of(t));
    this->nextFreeSymbolIndex -= size_of(t);

    return newTmpVar;
}

int CFG::get_var_index(string name)
{
    return this->symbolsTable->getSymbolIndex(name);
}

Type CFG::get_var_type(string name)
{
    return this->symbolsTable->getSymbolType(name);
}

string CFG::toRegister(string name)

{
    const int index = this->get_var_index(name);

    static const vector<string> regParams = {"%edi", "%esi", "%edx", "%ecx", "%r8", "%r9"};

    if (index > 0)
    {
        return regParams[index - 1];
    }

    if (name[0] == 't')
    {
        int index = -stoi(name.substr(3));
        return to_string(index) + "(%rbp)";
    }
    return to_string(index) + "(%rbp)";
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
void CFG::setSymbolsTable(SymbolsTable *symbolsTable)
{
    this->symbolsTable = symbolsTable;
}
