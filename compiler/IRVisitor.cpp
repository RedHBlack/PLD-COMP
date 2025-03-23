#include "IR/Instr/IRInstrLoadConst.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrArithmeticOp.h"
#include "IR/Instr/IRInstrUnaryOp.h"
#include "IR/Instr/IRInstrComp.h"
#include "IR/Instr/IRInstrClean.h"
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
    this->currentCFG = this->cfgs["main"];
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    // for each function_stmt
    // name = function_stmt->getName()->geText()
    // this->cfgs[name] = new CFG(name, symbolsTable, baseStackOffset - 4);
    this->currentCFG->add_bb(new BasicBlock(this->currentCFG, "body"));

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
        this->currentCFG->resetNextFreeSymbolIndex();
    }

    visit(ctx->return_stmt());

    BasicBlock *output = new BasicBlock(this->currentCFG, "output");
    output->add_IRInstr(new IRInstrClean(output));

    this->currentCFG->getCurrentBasicBlock()->setExitTrue(output);

    this->currentCFG->add_bb(output);

    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(exprCtx))
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), "%eax"));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), "%eax"));
    }
    else if (ctx->assign_stmt())
    {
        visit(ctx->assign_stmt());
        currentBB->add_IRInstr(new IRInstrMove(currentBB, ctx->assign_stmt()->VAR()->getText(), "%eax"));
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
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

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

antlrcpp::Any IRVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{

    handleArithmeticOp(ctx->expr(0), ctx->expr(1), ctx->OP->getText());

    return 0;
}

antlrcpp::Any IRVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{

    handleArithmeticOp(ctx->expr(0), ctx->expr(1), ctx->OP->getText());

    return 0;
}

antlrcpp::Any IRVisitor::visitBitwise(ifccParser::BitwiseContext *ctx)
{
    handleArithmeticOp(ctx->expr(0), ctx->expr(1), ctx->OP->getText());

    return 0;
}

antlrcpp::Any IRVisitor::visitComp(ifccParser::CompContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText();

    loadRegisters(ctx->expr(0), ctx->expr(1));

    currentBB->add_IRInstr(new IRInstrComp(currentBB, "%ebx", "%eax", op));

    return 0;
}

antlrcpp::Any IRVisitor::visitUnary(ifccParser::UnaryContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText();

    visitExpr(ctx->expr(), true);

    currentBB->add_IRInstr(new IRInstrUnaryOp(currentBB, "%eax", op));

    return 0;
}

antlrcpp::Any IRVisitor::visitPre(ifccParser::PreContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText();
    const int offsetVar = this->currentCFG->get_var_index(ctx->VAR()->getText());

    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", ctx->VAR()->getText(), op[0] + ""));

    return 0;
}

// INVALID
antlrcpp::Any IRVisitor::visitPost(ifccParser::PostContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    const string op = ctx->OP->getText();

    currentBB->add_IRInstr(new IRInstrMove(currentBB, ctx->VAR()->getText(), "%eax"));

    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", "%eax", op[0] + ""));

    return 0;
}

void IRVisitor::assignValueToVar(ifccParser::ExprContext *ctx, string varName)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

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
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

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
        const string newTmpVar = this->currentCFG->create_new_tempvar();

        visitExpr(leftExpr, true);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", newTmpVar));
        visitExpr(rightExpr, false);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, newTmpVar, "%eax"));
    }
}

void IRVisitor::gen_asm(ostream &o)
{
    this->currentCFG->gen_asm(o);
}

void IRVisitor::setCurrentCFG(CFG *currentCFG)
{
    this->currentCFG = currentCFG;
}

void IRVisitor::handleArithmeticOp(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr, string op)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    loadRegisters(leftExpr, rightExpr);

    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "%ebx", "%eax", op));
}

CFG *IRVisitor::getCurrentCFG()
{
    return this->currentCFG;
}

map<string, CFG *> IRVisitor::getCFGS()
{
    return cfgs;
}
