#include "IRInstrComp.h"

void IRInstrComp::gen_asm(ostream &o)
{
    o << "   cmpl " << firstOp << ", " << secondOp << "\n";

    if (op == "==")
    {
        handleCompEq(o);
    }
    else if (op == "!=")
    {
        handleCompNotEq(o);
    }
    else if (op == ">")
    {
        handleGreater(o);
    }
    else if (op == ">=")
    {
        handleGreaterOrEqual(o);
    }
    else if (op == "<")
    {
        handleLower(o);
    }
    else if (op == "<=")
    {
        handleLowerOrEqual(o);
    }

    o << "   movzbl %al," << secondOp << "\n";
}

void IRInstrComp::handleCompEq(ostream &o)
{
    o << "   sete %al\n";
}

void IRInstrComp::handleCompNotEq(ostream &o)
{
    o << "   setne %al\n";
}

void IRInstrComp::handleGreater(ostream &o)
{
    o << "   setg %al\n";
}

void IRInstrComp::handleGreaterOrEqual(ostream &o)
{
    o << "   setge %al\n";
}

void IRInstrComp::handleLower(ostream &o)
{
    o << "   setl %al\n";
}

void IRInstrComp::handleLowerOrEqual(ostream &o)
{
    o << "   setle %al\n";
}
