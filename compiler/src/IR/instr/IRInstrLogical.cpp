#include "IRInstrLogical.h"

int IRInstrLogical::label = 0;

void IRInstrLogical::genASM(ostream &o)
{
    if (op == "and")
    {
        handleLogicalAnd(o);
    }
    else if (op == "or")
    {
        handleLogicalOr(o);
    }
}

void IRInstrLogical::handleLogicalAnd(ostream &o)
{
    int falseLabel = label++;
    int trueLabel = label++;
    int finalLabel = label++;
    o << "   cmpl $0, " << firstOp << "\n";
    o << "   je .lbl" << falseLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   je .lbl" << falseLabel << "\n";
    o << "   movl $1, %eax \n";
    o << "   jmp .lbl" << trueLabel << "\n";
    o << ".lbl" << falseLabel << ":\n";
    o << "   movl $0, %eax \n";
    o << "   jmp .final" << finalLabel << "\n";
    o << ".lbl" << trueLabel << ":\n";
    o << "   jmp .final" << finalLabel << "\n";
    o << ".final" << finalLabel << ":\n";
}

void IRInstrLogical::handleLogicalOr(ostream &o)
{
    int falseLabel = label++;
    int trueLabel = label++;
    int finalLabel = label++;
    o << "   cmpl $0, " << firstOp << "\n";
    o << "   jne .lbl" << trueLabel << "\n";
    o << "   cmpl $0, " << secondOp << "\n";
    o << "   jne .lbl" << trueLabel << "\n";
    o << "   movl $0, %eax \n";
    o << "   jmp .lbl" << falseLabel << "\n";
    o << ".lbl" << trueLabel << ":\n";
    o << "   movl $1, %eax \n";
    o << "   jmp .final" << finalLabel << "\n";
    o << ".lbl" << falseLabel << ":\n";
    o << "jmp .final" << finalLabel << "\n";
    o << ".final" << finalLabel << ":\n";
}