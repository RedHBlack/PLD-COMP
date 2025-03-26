#include "IRInstrLogical.h"

int IRInstrLogical::label = 0;

void IRInstrLogical::gen_asm(ostream &o)
{
    if (op == "&&")
    {
        handleLogicalAnd(o);
    }
    else if (op == "||")
    {
        handleLogicalOr(o);
    }
}

void IRInstrLogical::handleLogicalAnd(ostream &o)
{
    int falseLabel = label++;
    int trueLabel = label++;
    o << "   cmpl $0, " << firstOp << "\n";
    o << "   je " << falseLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   je " << falseLabel << "\n";
    o << "   movl $1, %eax \n";
    o << "   jmp " << trueLabel << "\n";
    o << falseLabel << ":\n";
    o << "   movl $0, %eax \n";
    o << trueLabel << ":\n";
}

void IRInstrLogical::handleLogicalOr(ostream &o)
{
    int falseLabel = label++;
    int trueLabel = label++;
    o << "   cmpl $0, " << firstOp << "\n";
    o << "   jne " << trueLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   jne " << trueLabel << "\n";
    o << "   movl $0, %eax \n";
    o << "   jmp " << falseLabel << "\n";
    o << trueLabel << ":\n";
    o << "   movl $1, %eax \n";
    o << falseLabel << ":\n";
}