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
    std::cout << ".globl _main\n";
    std::cout << " _main: \n";
#else
    std::cout << ".globl main\n";
    std::cout << "main: \n";
#endif

    std::cout << "      pushq %rbp\n";
    std::cout << "      movq %rsp, %rbp\n";

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
        resetCurrentTemporaryOffset();
    }

    visit(ctx->return_stmt());

    std::cout << "      popq %rbp\n";
    std::cout << "      ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    ifccParser::ExprContext *exprCtx = ctx->expr();

    ifccParser::ConstContext *constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx);
    if (constCtx)
    {
        cout << "      movl $" << stoi(constCtx->CONST()->getText()) << ", %eax\n";
        return 0;
    }

    ifccParser::VarContext *varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx);
    if (varCtx)
    {
        string varName = varCtx->VAR()->getText();
        cout << "      movl -" << symbolsTable[varName] << "(%rbp), %eax\n";

        return 0;
    }

    visit(exprCtx);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    string varName = ctx->VAR()->getText();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    ifccParser::VarContext *varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx);
    if (varCtx != nullptr)
    {
        string assignedVarName = varCtx->VAR()->getText();

        cout << "      movl -" << symbolsTable[assignedVarName] << "(%rbp), %eax\n";
        cout << "      movl %eax, -" << symbolsTable[varName] << "(%rbp)\n";
    }
    else if (dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        cout << "      movl $" << stoi(dynamic_cast<ifccParser::ConstContext *>(exprCtx)->CONST()->getText()) << ", -" << symbolsTable[varName] << "(%rbp)\n";
    }
    else
    {
        visit(exprCtx);
        cout << "      movl	%eax,-" << symbolsTable[varName] << "(%rbp)\n";
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    return stoi(ctx->CONST()->getText());
}

antlrcpp::Any CodeGenVisitor::visitExpr(ifccParser::ExprContext *expr, bool isFirst)
{
    ifccParser::VarContext *varCtx = dynamic_cast<ifccParser::VarContext *>(expr);
    if (varCtx != nullptr)
    {
        string varName = varCtx->VAR()->getText();
        cout << "      movl -" << symbolsTable[varName] << "(%rbp), " << (isFirst ? "%eax" : "%ebx") << "\n";
        return 0;
    }

    ifccParser::ConstContext *constCtx = dynamic_cast<ifccParser::ConstContext *>(expr);
    if (constCtx != nullptr)
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

void CodeGenVisitor::resetCurrentTemporaryOffset()
{
    currentTemporaryOffset = maxOffset + 4;
}

antlrcpp::Any CodeGenVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    char op = ctx->OPA()->getText()[0];
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
    char op = ctx->OPM()->getText()[0];
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
    case '%':
        cout << "      cdq\n";
        cout << "      idivl %ebx\n";
        if (op == '%')
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