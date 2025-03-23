#include "IR/Instr/IRInstrLoadConst.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrAdd.h"
#include "IR/Instr/IRInstrSub.h"
#include "IR/Instr/IRInstrMult.h"
#include "IR/Instr/IRInstrDiv.h"
#include "IR/Instr/IRInstrMod.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrNot.h"
#include "IR/Instr/IRInstrNeg.h"
#include "IR/Instr/IRInstrComp.h"
#include "IR/Instr/IRInstrAnd.h"
#include "IR/Instr/IRInstrOr.h"
#include "IR/Instr/IRInstrXor.h"
#include "IRVisitor.h"
#include <iostream>
#include <map>
#include <string>
#include "IR/CFG.h"
#include "IR/BasicBlock.h"

using namespace std;

IRVisitor::IRVisitor(map<string, int> symbolsTable, int baseStackOffset)
{
    this->cfgs["main"] = new CFG("main", symbolsTable, baseStackOffset - 4);
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    this->cfgs["main"]->add_bb(new BasicBlock(this->cfgs["main"], "body"));

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
        this->cfgs["main"]->resetNextFreeSymbolIndex();
    }

    visit(ctx->return_stmt());

    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), "%eax"));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), "%eax"));
    }
    else
    {
        visitExpr(exprCtx, true);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;

    for (size_t i = 0; i < ctx->children.size(); i++)
    {
        antlr4::tree::TerminalNode *var = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i]);

        if (var && var->getSymbol()->getType() == ifccParser::VAR)
        {
            if (i + 1 < ctx->children.size())
            {
                antlr4::tree::TerminalNode *nextChild = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1]);

                if (nextChild && nextChild->getText() == "=")
                {
                    assignValueToVar(ctx->expr(exprIndex), var->getText());
                    exprIndex++;
                }
            }
        }
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    assignValueToVar(ctx->expr(), ctx->VAR()->getText());
    return 0;
}

antlrcpp::Any IRVisitor::visitExpr(ifccParser::ExprContext *expr, bool isFirst)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(expr))
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), (isFirst ? "%eax" : "%ebx")));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), (isFirst ? "%eax" : "%ebx")));
    }
    else
    {
        visit(expr);

        if (!isFirst)
        {
            currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", "%ebx"));
        }
    }

    return 0;
}

void IRVisitor::assignValueToVar(ifccParser::ExprContext *ctx, string varName)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), varName));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), varName));
    }
    else
    {
        visitExpr(ctx, true);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", varName));
    }
}

void IRVisitor::loadRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), "%eax"));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(leftExpr))
    {
        visitExpr(rightExpr, false);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), "%eax"));
    }
    else
    {
        const string newTmpVar = this->cfgs["main"]->create_new_tempvar();

        visitExpr(leftExpr, true);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", newTmpVar));
        visitExpr(rightExpr, false);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, newTmpVar, "%eax"));
    }
}
antlrcpp::Any IRVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    const char op = ctx->OP->getText()[0];
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    loadRegisters(ctx->expr(0), ctx->expr(1));

    if (op == '+')
    {
        currentBB->add_IRInstr(new IRInstrAdd(currentBB, "%ebx", "%eax"));
    }
    else if (op == '-')
    {
        currentBB->add_IRInstr(new IRInstrSub(currentBB, "%ebx", "%eax"));
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    const char op = ctx->OP->getText()[0];

    loadRegisters(ctx->expr(0), ctx->expr(1));

    switch (op)
    {
    case '*':
        currentBB->add_IRInstr(new IRInstrMult(currentBB, "%ebx", "%eax"));
        break;
    case '/':
        currentBB->add_IRInstr(new IRInstrDiv(currentBB, "%ebx", "%eax"));
        break;
    case '%':
        currentBB->add_IRInstr(new IRInstrMod(currentBB, "%ebx", "%eax"));
        break;
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitPre(ifccParser::PreContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    const char op = ctx->OPU()->getText()[0];
    const int offsetVar = this->cfgs["main"]->get_var_index(ctx->VAR()->getText());

    switch (op)
    {
    case '+':
        currentBB->add_IRInstr(new IRInstrAdd(currentBB, "%ebx", "%eax"));
        break;
    case '-':
        currentBB->add_IRInstr(new IRInstrSub(currentBB, "%ebx", "%eax"));
        break;
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitPost(ifccParser::PostContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();

    const char op = ctx->OPU()->getText()[0];
    const int offsetVar = this->cfgs["main"]->get_var_index(ctx->VAR()->getText());

    switch (op)
    {
    case '+':
        currentBB->add_IRInstr(new IRInstrAdd(currentBB, "%ebx", "%eax"));
        break;
    case '-':
        currentBB->add_IRInstr(new IRInstrSub(currentBB, "%ebx", "%eax"));
        break;
    }

    return 0;
}

void IRVisitor::gen_asm(ostream &o)
{
    this->cfgs["main"]->gen_asm(o);
}

antlrcpp::Any IRVisitor::visitNot(ifccParser::NotContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    visitExpr(ctx->expr(), true);

    currentBB->add_IRInstr(new IRInstrNot(currentBB, "%eax"));

    return 0;
}

antlrcpp::Any IRVisitor::visitNeg(ifccParser::NegContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    visitExpr(ctx->expr(), true);

    currentBB->add_IRInstr(new IRInstrNeg(currentBB, "%eax"));

    return 0;
}

antlrcpp::Any IRVisitor::visitBitBybit(ifccParser::BitBybitContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    const char op = ctx->OP->getText()[0];

    loadRegisters(ctx->expr(0), ctx->expr(1));

    switch (op)
    {
    case '&':
        currentBB->add_IRInstr(new IRInstrAnd(currentBB, "%ebx", "%eax"));

        break;
    case '|':
        currentBB->add_IRInstr(new IRInstrOr(currentBB, "%ebx", "%eax"));

        break;
    case '^':
        currentBB->add_IRInstr(new IRInstrXor(currentBB, "%ebx", "%eax"));
        break;
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitComp(ifccParser::CompContext *ctx)
{
    BasicBlock *currentBB = this->cfgs["main"]->getCurrentBasicBlock();
    const string op = ctx->OP->getText();

    loadRegisters(ctx->expr(0), ctx->expr(1));

    currentBB->add_IRInstr(new IRInstrComp(currentBB, "%ebx", "%eax", op));

    return 0;
}