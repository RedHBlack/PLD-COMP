#include "IRInstrComp.h"

void IRInstrComp::gen_asm(ostream &o)
{
    o << "   cmpl " << src << ", " << dest << "\n";

    if (op == "==")
    {
        o << "   sete %al\n";
    }
    else if (op == "!=")
    {
        o << "   setne %al\n";
    }
    else if (op == ">")
    {
        o << "   setg %al\n";
    }
    else if (op == "<")
    {
        o << "   setl %al\n";
    }
    else if (op == ">=")
    {
        o << "   setge %al\n";
    }
    else if (op == "<=")
    {
        o << "   setle %al\n";
    }

    o << "   movzbl %al," << dest << "\n";
}