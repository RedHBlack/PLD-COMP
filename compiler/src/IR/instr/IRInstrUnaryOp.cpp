#include "IRInstrUnaryOp.h"

void IRInstrUnaryOp::gen_asm(ostream &o)
{
    if (op == "!")
        handleNot(o);
    else if (op == "-")
        handleNeg(o);
    else if (op == "~")
        handleCompl(o);
    else if (op == "cltq")
        o << "   cltq\n";
        
    // else if (op == "&")
    //     handleReference(o);
    // else if (op == "*")
    //     handlePointer(o);
}

void IRInstrUnaryOp::handleNot(ostream &o)
{
    o << "   testl " << uniqueOp << ", " << uniqueOp << "\n";
    o << "   movl $0, " << uniqueOp << "\n";
    o << "   sete %al\n";
    o << "   movzbl %al, " << uniqueOp << "\n";
}

void IRInstrUnaryOp::handleNeg(ostream &o)
{
    o << "   negl " << uniqueOp << "\n";
}

void IRInstrUnaryOp::handleCompl(ostream &o)
{
    o << "not " << uniqueOp << "\n";
}
