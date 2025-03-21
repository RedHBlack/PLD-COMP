#include "CodeGenVisitor.h"
#include <iostream>

using namespace std;

CodeGenVisitor::CodeGenVisitor(map<string, int> symbolsTable, int maxOffset)
{
    this->symbolsTable = symbolsTable;
    this->maxOffset = maxOffset;
    this->currentTemporaryOffset = maxOffset - 4;
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
#ifdef __APPLE__
    cout << ".globl _main\n";
    cout << " _main:\n";
#else
    cout << ".globl main\n";
    cout << "main:\n";
#endif

    cout << "   pushq %rbp\n";
    cout << "   movq %rsp, %rbp\n";

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
        resetCurrentTemporaryOffset();
    }

    visit(ctx->return_stmt());

    cout << "   popq %rbp\n";
    cout << "   ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    ifccParser::ExprContext *exprCtx = ctx->expr();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        cout << "   movl $" << constCtx->CONST()->getText() << ", %eax\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        const int offsetVar = this->symbolsTable[varCtx->VAR()->getText()];
        cout << "   movl " << offsetVar << "(%rbp), %eax\n";
    }
    else
    {
        visitExpr(exprCtx, true);
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;

    for (int i = 0; i < ctx->children.size(); i++)
    {
        antlr4::tree::TerminalNode *var = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i]);

        if (var && var->getSymbol()->getType() == ifccParser::VAR)
        {
            if (i + 1 < ctx->children.size())
            {
                antlr4::tree::TerminalNode *nextChild = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1]);

                if (nextChild && nextChild->getText() == "=")
                {
                    saveValueInStack(ctx->expr(exprIndex), var->getText());
                    exprIndex++;
                }
            }
        }
    }

    return 0;
}
antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{

    saveValueInStack(ctx->expr(), ctx->VAR()->getText());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr(ifccParser::ExprContext *expr, bool isFirst)
{

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(expr))
    {
        cout << "   movl $" << constCtx->CONST()->getText() << ", " << (isFirst ? "%eax" : "%ebx") << "\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        const int offsetVar = this->symbolsTable[varCtx->VAR()->getText()];
        cout << "   movl " << offsetVar << "(%rbp), " << (isFirst ? "%eax" : "%ebx") << "\n";
    }
    else
    {

        visit(expr);

        if (!isFirst)
        {
            cout << "   movl %eax, %ebx\n";
        }
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    const char op = ctx->OP->getText()[0];

    loadRegisters(ctx->expr(0), ctx->expr(1));

    switch (op)
    {
    case '+':
        cout << "   addl    %ebx, %eax\n";
        break;
    case '-':
        cout << "   subl    %ebx, %eax\n";
        break;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    const char op = ctx->OP->getText()[0];

    loadRegisters(ctx->expr(0), ctx->expr(1));

    switch (op)
    {
    case '*':
        cout << "   imull   %ebx, %eax\n";
        break;
    case '/':
        cout << "   cdq\n";
        cout << "   idivl %ebx\n";
        break;
    case '%':
        cout << "   cdq\n";
        cout << "   idivl %ebx\n";
        cout << "   movl %edx, %eax\n";
        break;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitPre(ifccParser::PreContext *ctx)
{
    const char op = ctx->OPU()->getText()[0];
    const int offsetVar = this->symbolsTable[ctx->VAR()->getText()];

    switch (op)
    {
    case '+':
        cout << "   addl    $1, " << offsetVar << "(%rbp)\n";
        break;
    case '-':
        cout << "   subl    $1, " << offsetVar << "(%rbp)\n";
        break;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitPost(ifccParser::PostContext *ctx)
{
    const char op = ctx->OPU()->getText()[0];
    const int offsetVar = this->symbolsTable[ctx->VAR()->getText()];

    switch (op)
    {
    case '+':
        cout << "   addl    $1, " << offsetVar << "(%rbp)\n";
        break;
    case '-':
        cout << "   subl    $1, " << offsetVar << "(%rbp)\n";
        break;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNot(ifccParser::NotContext *ctx)
{

    visitExpr(ctx->expr(), true);

    cout << "   testl %eax, %eax\n";
    cout << "   movl $0, %eax\n";
    cout << "   sete %al\n";
    cout << "   movzbl %al, %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNeg(ifccParser::NegContext *ctx)
{
    visitExpr(ctx->expr(), true);

    cout << "   negl %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitBitBybit(ifccParser::BitBybitContext *ctx)
{
    const char op = ctx->OP->getText()[0];

    loadRegisters(ctx->expr(0), ctx->expr(1));

    switch (op)
    {
    case '&':
        cout << "   andl %ebx, %eax\n";
        break;
    case '|':
        cout << "   orl %ebx, %eax\n";
        break;
    case '^':
        cout << "   xorl %ebx, %eax\n";
        break;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitComp(ifccParser::CompContext *ctx)
{
    const string op = ctx->OP->getText();

    loadRegisters(ctx->expr(0), ctx->expr(1));

    cout << "   cmpl %ebx, %eax\n";

    if (op == "==")
    {
        cout << "   sete %al\n";
    }
    else if (op == "!=")
    {
        cout << "   setne %al\n";
    }
    else if (op == ">")
    {
        cout << "   setg %al\n";
    }
    else if (op == "<")
    {
        cout << "   setl %al\n";
    }

    cout << "   movzbl %al, %eax\n";

    return 0;
}

void CodeGenVisitor::saveValueInStack(ifccParser::ExprContext *ctx, string varName)
{
    const int offsetVarInStack = this->symbolsTable[varName];

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        cout << "   movl $" << constCtx->CONST()->getText() << ", " << offsetVarInStack << "(%rbp)\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx))
    {
        const int varIndex = this->symbolsTable[varCtx->getText()];
        cout << "   movl " << varIndex << "(%rbp), %eax\n";
        cout << "   movl %eax, " << offsetVarInStack << "(%rbp)\n";
    }
    else
    {
        visitExpr(ctx, true);
        cout << "   movl %eax, " << offsetVarInStack << "(%rbp)\n";
    }
}

void CodeGenVisitor::loadRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr)
{

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        cout << "   movl $" << constCtx->CONST()->getText() << ", %eax\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        cout << "   movl " << this->symbolsTable[varCtx->VAR()->getText()] << "(%rbp), %eax\n";
    }
    else
    {
        visitExpr(leftExpr, true);
        cout << "   movl %eax, " << this->currentTemporaryOffset << "(%rbp)\n";
        visitExpr(rightExpr, false);
        cout << "   movl " << this->currentTemporaryOffset << "(%rbp), %eax\n";

        this->currentTemporaryOffset -= 4;
    }
}

void CodeGenVisitor::resetCurrentTemporaryOffset()
{
    this->currentTemporaryOffset = this->maxOffset - 4;
}