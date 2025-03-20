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
        cout << "   movl $" << stoi(constCtx->CONST()->getText()) << ", %eax\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        string varName = varCtx->VAR()->getText();
        cout << "   movl " << this->symbolsTable[varName] << "(%rbp), %eax\n";
    }

    visitExpr(exprCtx, true);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    // On parcourt les enfants de la déclaration
    // La grammaire est : TYPE VAR ('=' expr)? (',' VAR ('=' expr)?)* ';'
    // On va donc examiner chaque enfant afin de repérer les variables et vérifier si elles ont un "=" juste après.
    for (int i = 0; i < ctx->children.size(); i++)
    {
        // On teste si l'enfant est un token (TerminalNode) et correspond à une variable

        if (auto terminal = dynamic_cast<ifccParser::VarContext *>(ctx->children[i]))
        {
            string var = terminal->getText();
            int index = this->symbolsTable[var];

            // Vérifier si le prochain enfant existe et est le token "="
            if (i + 1 < ctx->children.size())
            {
                auto nextChild = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1]);
                if (nextChild != nullptr && nextChild->getText() == "=")
                {
                    // L'initialiseur est associé à cette variable.
                    // On récupère l'expression correspondante.
                    // Dans le parse tree, les expressions initialisatrices sont collectées dans ctx->expr()
                    // dans l'ordre où elles apparaissent.
                    // On peut utiliser un compteur séparé pour parcourir ces expressions.
                    static int exprIndex = 0; // attention : si la visite est réutilisée ailleurs, mieux vaut déclarer exprIndex en variable locale à la fonction
                    if (exprIndex < ctx->expr().size())
                    {
                        ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);
                        exprIndex++; // passe à l'initialiseur suivant pour la prochaine variable

                        // Traitement selon que l'initialiseur est une constante ou une variable
                        if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
                        {
                            int valeur = stoi(constCtx->getText());
                            std::cout << "   movl $" << valeur << ", " << this->symbolsTable[var] << "(%rbp)\n";
                        }
                        else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
                        {
                            std::cout << "   movl " << this->symbolsTable[varCtx->getText()] << "(%rbp), %eax\n";
                            std::cout << "   movl %eax, " << this->symbolsTable[var] << "(%rbp)\n";
                        }
                        else
                        {
                            visitExpr(exprCtx, true);
                            cout << "   movl %eax, " << this->symbolsTable[var] << "(%rbp)\n";
                        }
                    }
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
        int val = stoi(constCtx->CONST()->getText());
        cout << "   movl $" << val << ", " << (isFirst ? "%eax" : "%ebx") << "\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        string varName = varCtx->VAR()->getText();
        cout << "   movl " << this->symbolsTable[varName] << "(%rbp), " << (isFirst ? "%eax" : "%ebx") << "\n";
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
    char op = ctx->OP->getText()[0];

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
    char op = ctx->OP->getText()[0];

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
    char op = ctx->OPU()->getText()[0];
    string varName = ctx->VAR()->getText();

    switch (op)
    {
    case '+':
        cout << "   addl    $1, " << this->symbolsTable[varName] << "(%rbp)\n";
        break;
    case '-':
        cout << "   subl    $1, " << this->symbolsTable[varName] << "(%rbp)\n";
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
        cout << "   addl    $1, " << this->symbolsTable[varName] << "(%rbp)\n";
        break;
    case '-':
        cout << "   subl    $1, " << this->symbolsTable[varName] << "(%rbp)\n";
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
    char op = ctx->OP->getText()[0];

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
    string op = ctx->OP->getText();

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
    const int indexVarInStack = this->symbolsTable[varName];

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        int value = stoi(constCtx->getText());
        cout << "   movl $" << value << ", " << indexVarInStack << "(%rbp)\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx))
    {
        int varIndex = this->symbolsTable[varCtx->getText()];
        cout << "   movl " << varIndex << "(%rbp), %eax\n";
        cout << "   movl %eax, " << indexVarInStack << "(%rbp)\n";
    }
    else
    {
        visitExpr(ctx, true);
        cout << "   movl %eax, " << indexVarInStack << "(%rbp)\n";
    }
}

void CodeGenVisitor::loadRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr)
{

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        cout << "   movl $" << stoi(dynamic_cast<ifccParser::ConstContext *>(leftExpr)->CONST()->getText()) << ", %eax\n";
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        cout << "   movl " << this->symbolsTable[dynamic_cast<ifccParser::VarContext *>(leftExpr)->VAR()->getText()] << "(%rbp), %eax\n";
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