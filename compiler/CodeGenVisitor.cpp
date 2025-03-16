#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(map<string, int> symbolsTable)
{
    this->symbolsTable = symbolsTable;
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

    this->visitChildren(ctx);

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
        cout << "      movl $" << any_cast<int>(visit(exprCtx)) << ", %eax\n";
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
        cout << "      movl $" << any_cast<int>(visit(exprCtx)) << ", -" << symbolsTable[varName] << "(%rbp)\n";
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
    return (int)stoi(ctx->CONST()->getText());
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
        int val = any_cast<int>(visit(expr));
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
    char op = ctx->OPA()->getText()[0];
    bool isLeftConst = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(0)) != nullptr;

    if (isLeftConst)
    {
        visitExpr(ctx->expr(1), false);
        cout << "      movl $" << any_cast<int>(visit(ctx->expr(0))) << ", %eax\n";
    }
    else
    {
        visitExpr(ctx->expr(0), true);
        cout << "      subq $4, %rsp\n";
        cout << "      movl %eax, (%rsp)\n";
        visitExpr(ctx->expr(1), false);
        cout << "      movl (%rsp), %eax\n";
        cout << "      addq $4, %rsp\n";
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
        cout << "      movl $" << any_cast<int>(visit(ctx->expr(0))) << ", %eax\n";
    }
    else
    {
        visitExpr(ctx->expr(0), true);
        cout << "      subq $4, %rsp\n";
        cout << "      movl %eax, (%rsp)\n";
        visitExpr(ctx->expr(1), false);
        cout << "      movl (%rsp), %eax\n";
        cout << "      addq $4, %rsp\n";
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

std::any CodeGenVisitor::visitPre(ifccParser::PreContext *ctx)
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

std::any CodeGenVisitor::visitPost(ifccParser::PostContext *ctx)
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
