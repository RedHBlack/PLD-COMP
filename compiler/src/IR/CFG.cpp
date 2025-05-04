#include "CFG.h"
#include "../utils/TypeManager.h"
#include "instr/IRInstrSet.h"
#include <sstream>
#include "instr/IRInstrJmpCond.h"
#include "instr/IRInstrJmpRet.h"

CFG::CFG(string label, SymbolsTable *symbolsTable, int initialNextFreeSymbolIndex) : label(label), symbolsTable(symbolsTable), nextFreeSymbolIndex(initialNextFreeSymbolIndex), initialTempPos(initialNextFreeSymbolIndex)
{
    idBB = 0;
}

CFG::~CFG()
{
    for (BasicBlock *bb : bbs)
    {
        delete bb;
    }
}
void CFG::addBB(BasicBlock *bb)
{
    bbs.push_back(bb);
    this->setCurrentBasicBlock(bb);
}

void CFG::genASM(ostream &o)
{
    for (int i = 0; i < bbs.size(); i++)
    {
        o << bbs[i]->getLabel() << ":" << endl;
        bbs[i]->genASM(o);
    }
}

string CFG::createNewTempvar(Type t)
{
    const string newTmpVar = "tmp" + to_string(-nextFreeSymbolIndex);

    this->symbolsTable->addSymbol(newTmpVar, t, TypeManager::size_of(t));
    this->nextFreeSymbolIndex -= TypeManager::size_of(t);

    return newTmpVar;
}

int CFG::getVarIndex(string name)
{
    return this->symbolsTable->getSymbolIndex(name);
}

Type CFG::getVarType(string name)
{
    return this->symbolsTable->getSymbolType(name);
}

string CFG::toRegister(string name)

{
    const int index = this->getVarIndex(name);

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

void CFG::genCFGGraphviz(ostream &o)
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
            instr->genASM(temp);

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

void CFG::addIfThenElse(BasicBlock *test, BasicBlock *then_bb, BasicBlock *else_bb, BasicBlock *end_bb)
{
    // Add conditional jumps from the test block
    test->addIRInstr(new IRInstrJmpCond(test, "je", else_bb->getLabel(), "edx"));

    // At the end of the then block, jump to end_bb
    then_bb->addIRInstr(new IRInstrJmpRet(then_bb, end_bb->getLabel()));

    // No need to jump at the end of the else block because the control flow continues naturally
}

void CFG::addWhile(BasicBlock *test, BasicBlock *body, BasicBlock *end_bb)
{
    // The test jumps to end_bb if the condition is false
    test->addIRInstr(new IRInstrJmpCond(test, "je", end_bb->getLabel(), "edx"));

    // At the end of the body, jump to test
    body->addIRInstr(new IRInstrJmpRet(body, test->getLabel()));
}
void CFG::setSymbolsTable(SymbolsTable *symbolsTable)
{
    this->symbolsTable = symbolsTable;
}
