#include "IRInstrArithmeticOp.h"

void IRInstrArithmeticOp::genASM(ostream &o)
{
    switch (op[0])
    {
    case '+':
        handleAddition(o);
        break;
    case '-':
        handleSubstraction(o);
        break;
    case '*':
        handleMult(o);
        break;
    case '/':
        handleDiv(o);
        break;
    case '%':
        handleModulo(o);
        break;
    case '&':
        handleBitwiseAnd(o);
        break;
    case '|':
        handleBitwiseOr(o);
        break;
    case '^':
        handleBitwiseXor(o);
        break;
    case '<':
        handleLeftShift(o);
        break;
    case '>':
        handleRightShift(o);
        break;
    }
}

void IRInstrArithmeticOp::handleAddition(ostream &o)
{
    o << "   addl " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleSubstraction(ostream &o)
{
    o << "   subl " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleMult(ostream &o)
{
    o << "   imull " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleDiv(ostream &o)
{
    o << "   cdq\n";
    o << "   idivl " << firstOp << "\n";
    if (secondOp != "%eax")
        o << "   movl %eax" << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleModulo(ostream &o)
{
    o << "   cdq\n";
    o << "   idivl " << firstOp << "\n";
    o << "   movl %edx" << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleBitwiseAnd(ostream &o)
{
    o << "   andl " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleBitwiseOr(ostream &o)
{
    o << "   orl " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleBitwiseXor(ostream &o)
{
    o << "   xorl " << firstOp << ", " << secondOp << "\n";
}

void IRInstrArithmeticOp::handleLeftShift(ostream &o)
{
    o << "   movl " << firstOp << ", %ecx\n";
    o << "   sall %cl," << secondOp << "\n";
    o << "   movl " << secondOp << ", %eax\n";
}

void IRInstrArithmeticOp::handleRightShift(ostream &o)
{
    o << "   movl " << firstOp << ", %ecx\n";
    o << "   sarl %cl," << secondOp << "\n";
    o << "   movl " << secondOp << ", %eax\n";
}