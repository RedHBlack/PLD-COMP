#include "CodeGenVisitor.h"
#include <iostream>
#include <string>
#include <map>
#include "antlr4-runtime.h"

using namespace std;

CodeGenVisitor::CodeGenVisitor(map<string, int> symbolsTable, int maxOffset)
{
    this->symbolsTable = symbolsTable;
    this->maxOffset = maxOffset;
    currentTemporaryOffset = maxOffset + 4;
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

    cout << "      pushq %rbp\n";
    cout << "      movq %rsp, %rbp\n";

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
        resetCurrentTemporaryOffset();
    }

    visit(ctx->return_stmt());

    cout << "      popq %rbp\n";
    cout << "      ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    ifccParser::ExprContext *exprCtx = ctx->expr();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        cout << "      movl $" << stoi(constCtx->CONST()->getText()) << ", %eax\n";
        return 0;
    }

    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        string varName = varCtx->VAR()->getText();
        cout << "      movl -" << symbolsTable[varName] << "(%rbp), %eax\n";
        return 0;
    }

    visitExpr(exprCtx, true);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    string varName = ctx->VAR()->getText();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        string assignedVarName = varCtx->VAR()->getText();
        cout << "      movl -" << symbolsTable[assignedVarName] << "(%rbp), %eax\n";
        cout << "      movl %eax, -" << symbolsTable[varName] << "(%rbp)\n";
    }
    else if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        cout << "      movl $" << stoi(constCtx->CONST()->getText()) << ", -" << symbolsTable[varName] << "(%rbp)\n";
    }
    else
    {
        visitExpr(exprCtx, true);
        cout << "      movl %eax, -" << symbolsTable[varName] << "(%rbp)\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    return stoi(ctx->CONST()->getText());
}

antlrcpp::Any CodeGenVisitor::visitExpr(ifccParser::ExprContext *expr, bool isFirst)
{
    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        string varName = varCtx->VAR()->getText();
        cout << "      movl -" << symbolsTable[varName] << "(%rbp), " << (isFirst ? "%eax" : "%ebx") << "\n";
        return 0;
    }

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(expr))
    {
        int val = stoi(constCtx->CONST()->getText());
        cout << "      movl $" << val << ", " << (isFirst ? "%eax" : "%ebx") << "\n";
        return 0;
    }

    visit(expr);
    if (!isFirst)
    {
        cout << "      movl %eax, %ebx\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    char op = ctx->OP->getText()[0];
    bool isLeftConst = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0)) != nullptr;

    if (isLeftConst)
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl $" << stoi(dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0))->CONST()->getText()) << ", %eax\n";
    }
    else if (dynamic_cast<ifccParser::VarContext *>(ctx->expr(0)))
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << symbolsTable[dynamic_cast<ifccParser::VarContext *>(ctx->expr(0))->VAR()->getText()] << "(%rbp), %eax\n";
    }
    else
    {
        visitExpr(ctx->expr(0), true);
        cout << "      movl %eax,-" << currentTemporaryOffset << "(%rbp)\n";
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << currentTemporaryOffset << "(%rbp), %eax\n";

        currentTemporaryOffset += 4;
    }

    switch (op)
    {
    case '+':
        cout << "      addl %ebx, %eax\n";
        break;
    case '-':
        cout << "      subl %ebx, %eax\n";
        break;
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    char op = ctx->OP->getText()[0];
    bool isLeftConst = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0)) != nullptr;

    if (isLeftConst)
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl $" << stoi(dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0))->CONST()->getText()) << ", %eax\n";
    }
    else if (dynamic_cast<ifccParser::VarContext *>(ctx->expr(0)))
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << symbolsTable[dynamic_cast<ifccParser::VarContext *>(ctx->expr(0))->VAR()->getText()] << "(%rbp), %eax\n";
    }
    else
    {
        visitExpr(ctx->expr(0), true);
        cout << "      movl %eax,-" << currentTemporaryOffset << "(%rbp)\n";
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << currentTemporaryOffset << "(%rbp), %eax\n";
    }

    switch (op)
    {
    case '*':
        cout << "      imull %ebx, %eax\n";
        break;
    case '/':
        cout << "      cdq\n";
        cout << "      idivl %ebx\n";
        break;
    case '%':
        cout << "      cdq\n";
        cout << "      idivl %ebx\n";
        cout << "      movl %edx, %eax\n";
        break;
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitPre(ifccParser::PreContext *ctx)
{
    char op = ctx->OPU()->getText()[0];
    string varName = ctx->VAR()->getText();

    switch (op)
    {
    case '+':
        cout << "      addl $1, -" << symbolsTable[varName] << "(%rbp)\n";
        break;
    case '-':
        cout << "      subl $1, -" << symbolsTable[varName] << "(%rbp)\n";
        break;
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitPost(ifccParser::PostContext *ctx)
{
    char op = ctx->OPU()->getText()[0];
    string varName = ctx->VAR()->getText();

    switch (op)
    {
    case '+':
        cout << "      addl $1, -" << symbolsTable[varName] << "(%rbp)\n";
        break;
    case '-':
        cout << "      subl $1, -" << symbolsTable[varName] << "(%rbp)\n";
        break;
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNot(ifccParser::NotContext *ctx)
{
    visitExpr(ctx->expr(), true);

    cout << "      testl %eax, %eax\n";
    cout << "      movl $0, %eax\n";
    cout << "      sete %al\n";
    cout << "      movzbl %al, %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNeg(ifccParser::NegContext *ctx)
{
    visitExpr(ctx->expr(), true);
    cout << "      negl %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitBitBybit(ifccParser::BitBybitContext *ctx)
{
    char op = ctx->OP->getText()[0];
    bool isLeftConst = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0)) != nullptr;

    if (isLeftConst)
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl $" << stoi(dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0))->CONST()->getText()) << ", %eax\n";
    }
    else if (dynamic_cast<ifccParser::VarContext *>(ctx->expr(0)))
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << symbolsTable[dynamic_cast<ifccParser::VarContext *>(ctx->expr(0))->VAR()->getText()] << "(%rbp), %eax\n";
    }
    else
    {
        visitExpr(ctx->expr(0), true);
        cout << "      movl %eax,-" << currentTemporaryOffset << "(%rbp)\n";
        visitExpr(ctx->expr(1), false);
        cout << "      movl -" << currentTemporaryOffset << "(%rbp), %eax\n";
    }

    switch (op)
    {
    case '&':
        cout << "      andl %ebx, %eax\n";
        break;
    case '|':
        cout << "      orl %ebx, %eax\n";
        break;
    case '^':
        cout << "      xorl %ebx, %eax\n";
        break;
    }

    return 0;
}

void CodeGenVisitor::resetCurrentTemporaryOffset()
{
    currentTemporaryOffset = maxOffset + 4;
}

// std::any CodeGenVisitor::visitOpposite(ifccParser::OppositeContext *ctx)
// {
//     ifccParser::ConstContext *constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx->expr());
//     if (constCtx != nullptr)
//     {
//         return -any_cast<int>(visit(ctx->expr()));
//     }

//     visitExpr(ctx->expr(), true);
//     cout << "      negl %eax\n";

//     return 0;
// }
