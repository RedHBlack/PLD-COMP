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
    o << "   je .lbl" << falseLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   je .lbl" << falseLabel << "\n";
    o << "   movl $1, %eax \n";
    o << "   jmp .lbl" << trueLabel << "\n";
    o << ".lbl" << falseLabel << ":\n";
    o << "   movl $0, %eax \n";
    o << ".lbl" << trueLabel << ":\n";
}

void IRInstrLogical::handleLogicalOr(ostream &o)
{
    int falseLabel = label++;
    int trueLabel = label++;
    o << "   cmpl $0, " << firstOp << "\n";
    o << "   jne .lbl" << trueLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   jne .lbl" << trueLabel << "\n";
    o << "   movl $0, %eax \n";
    o << "   jmp .lbl" << falseLabel << "\n";
    o << ".lbl" << trueLabel << ":\n";
    o << "   movl $1, %eax \n";
    o << ".lbl" << falseLabel << ":\n";
}