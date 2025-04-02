#include "IR/Instr/IRInstrLoadConst.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrArithmeticOp.h"
#include "IR/Instr/IRInstrUnaryOp.h"
#include "IR/Instr/IRInstrComp.h"
#include "IR/Instr/IRInstrClean.h"
#include "IR/Instr/IRInstrIf.h"
#include "IR/Instr/IRAfterIf.h"
#include "IR/Instr/IRInstrJump.h"
#include "IRVisitor.h"
#include <iostream>
#include <map>
#include <string>
#include "IR/CFG.h"
#include "IR/BasicBlock.h"

using namespace std;

IRVisitor::IRVisitor(map<string, int> symbolsTable, map<string, Type> symbolsType, int baseStackOffset)
{
    this->cfgs["main"] = new CFG("main", symbolsTable, symbolsType, baseStackOffset - 4);
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
    else
    {
        visit(exprCtx);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;
    // Parcourir tous les enfants du nœud de déclaration
    for (size_t i = 0; i < ctx->children.size(); i++)
    {
        // On cherche un token de type VAR
        auto varNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i]);
        if (varNode && varNode->getSymbol()->getType() == ifccParser::VAR)
        {
            string varName = varNode->getText();
            // Vérifier si le token suivant est "="
            if (i + 1 < ctx->children.size())
            {
                auto nextNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1]);
                if (nextNode && nextNode->getText() == "=")
                {
                    // La variable a bien un initialiseur, on utilise ctx->expr(exprIndex)
                    assignValueToVar(ctx->expr(exprIndex), varName);
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

antlrcpp::Any IRVisitor::visitAssign(ifccParser::AssignContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    if (auto innerAssign = dynamic_cast<ifccParser::AssignContext *>(ctx->expr()))
    {
        visitAssign(innerAssign);
    }
    else
    {
        visitExpr(ctx->expr(), true);
    }

    string varName = ctx->VAR()->getText();
    currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", varName));

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
    else if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(expr))
    {
        if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(assignCtx->VAR()))
        {
            visitExpr(assignCtx->expr(), false);
            currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", assignCtx->VAR()->getText()));
        }
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

antlrcpp::Any IRVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    BasicBlock *ifBB = new BasicBlock(this->currentCFG, "if_block");
    BasicBlock *elseIfBB = new BasicBlock(this->currentCFG, "else_if_block");;
    BasicBlock *elseBB = nullptr;
    BasicBlock *endBB = new BasicBlock(this->currentCFG, "end_if");
    /*
    // Visiter l'expression conditionnelle et récupérer le registre du résultat
    ifBB->add_IRInstr(new IRInstrIf(ifBB, "%ebx", "%eax",  "!=", ifBB->getLabel()));
    this->currentCFG->add_bb(ifBB);
    this->currentCFG->setCurrentBasicBlock(ifBB);

    // Visit the main if block
    //visit(ctx->if_block()->expr());

    //ifBB->add_IRInstr(new IRInstrIf(ifBB, "!=", ifBB->getLabel()));

    //this->currentCFG->add_bb(ifBB);
    //this->currentCFG->setCurrentBasicBlock(ifBB);

    // Gérer les else-if
    vector<BasicBlock *> elseIfBlocks;
    for (size_t i = 0; i < ctx->else_if_block().size(); i++) {
        elseIfBB = new BasicBlock(this->currentCFG, "else_if_block");
        //string condReg = std::any_cast<string>(visit(ctx->else_if_block(i)->expr()));
        elseIfBB->add_IRInstr(new IRInstrIf(elseIfBB, "%ebx", "%eax",  "==",elseIfBB->getLabel()));
        this->currentCFG->add_bb(elseIfBB);
        this->currentCFG->setCurrentBasicBlock(elseIfBB);
        elseIfBlocks.push_back(elseIfBB);
    }
    /*
    // Visit all else-if blocks
    for (size_t i = 0; i < ctx->else_if_block().size(); i++)
    {
        BasicBlock* elseIfBB = new BasicBlock(this->currentCFG, "else_if_block");

        visit(ctx->else_if_block(i)->expr());

        elseIfBB->add_IRInstr(new IRInstrIf(elseIfBB, "==", elseIfBB->getLabel()));

        this->currentCFG->add_bb(elseIfBB);
        //this->currentCFG->getCurrentBasicBlock()->setExitTrue(elseIfBB);
    }

    // Visit the else block if it exists
    if (ctx->else_block())
    {
        BasicBlock *elseBB = new BasicBlock(this->currentCFG, "else_block");

        this->currentCFG->add_bb(elseBB);

        elseBB->add_IRInstr(new IRInstrIf(elseBB, "", elseBB->getLabel()));
    }

    // Visit the if block
    visit(ctx->if_block());

    for(size_t i = 0; i < ctx->else_if_block().size(); i++)
    {
        visit(ctx->else_if_block(i));
    }

    visit(ctx->else_block());
    currentBB = this->currentCFG->getCurrentBasicBlock();
    currentBB->add_IRInstr(new IRAfterIf(currentBB, "end"));
    */

    vector<BasicBlock *> elseIfBlocks;

   // visit des expressions
    visit(ctx->if_block());
    
    
    currentBB->setExitTrue(ifBB);
    
    
    
    //visit des statements

    visit(ctx->if_block()->if_stmt_block());
    for(size_t i = 0; i < ctx->else_if_block().size(); i++)
    {
        visit(ctx->else_if_block(i)->else_if_stmt_block());
    }
    //visit(ctx->else_if_stmt_block());
    visit(ctx->else_block());


    // Gérer le else s'il existe
    /*if (ctx->else_block()) {
        elseBB = new BasicBlock(this->currentCFG, "else_block");
        this->currentCFG->add_bb(elseBB);
        this->currentCFG->setCurrentBasicBlock(elseBB);
    }*/

    // Lier les blocs conditionnels
    

    // Visiter les blocs
    /*visit(ctx->if_block());
    for (size_t i = 0; i < ctx->else_if_block().size(); i++) {
        visit(ctx->else_if_block(i));
    }
    if (ctx->else_block()) {
        visit(ctx->else_block());
    }

    this->currentCFG->add_bb(endBB);
    endBB->add_IRInstr(new IRAfterIf(endBB, "end_if"));*/
    return 0;
}

antlrcpp::Any IRVisitor::visitIf_block(ifccParser::If_blockContext *ctx)
{
    visit(ctx->if_block()->if_expr_block());

    for(size_t i = 0; i < ctx->else_if_block().size(); i++)
    {
        visit(ctx->else_if_block(i)->else_if_expr_block());
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitIf_expr_block(ifccParser::If_expr_blockContext *ctx)
{   
    BasicBlock *ifBB = new BasicBlock(this->currentCFG, "if_block");

    ifBB->setLabel("if_block");

    ifBB->add_IRInstr(new IRInstrIf(ifBB, "%ebx", "%eax",  "!=", ifBB->getLabel()));
    this->currentCFG->add_bb(ifBB);
    this->currentCFG->setCurrentBasicBlock(ifBB);
    return 0;
}

antlrcpp::Any IRVisitor::visitIf_stmt_block(ifccParser::If_stmt_blockContext *ctx)
{
    BasicBlock *ifBB = this->currentCFG->getCurrentBasicBlock();

    ifBB->add_IRInstr(new IRAfterIf(ifBB, ifBB->getLabel()));

    for (size_t i = 0; i < ctx->statement().size(); i++) {
        visit(ctx->statement(i));
    }
    if (ctx->return_stmt()) {
        visit(ctx->return_stmt());
    } else {
        ifBB->add_IRInstr(new IRInstrJump(ifBB, "end_if"));
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitElse_if_block(ifccParser::Else_if_blockContext *ctx)
{
    BasicBlock *elseIfBB = new BasicBlock(this->currentCFG, "else_if_block");
    BasicBlock *ifBB = this->currentCFG->getCurrentBasicBlock();
    BasicBlock *elseBB = nullptr;

    vector<BasicBlock *> elseIfBlocks;
    for (size_t i = 0; i < elseIfBlocks.size(); i++) {
        if (i == 0) {
            ifBB->setExitFalse(elseIfBlocks[i]);
        } else {
            elseIfBlocks[i - 1]->setExitFalse(elseIfBlocks[i]);
        }
    }
    if (!elseIfBlocks.empty()) {
        elseIfBlocks.back()->setExitFalse(elseBB ? elseBB : endBB);
    } else {
        ifBB->setExitFalse(elseBB ? elseBB : endBB);
    }
    if (elseBB) {
        elseBB->setExitFalse(endBB);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitElse_if_expr_block(ifccParser::Else_if_expr_blockContext *ctx)
{
    BasicBlock *elseIfBB = new BasicBlock(this->currentCFG, "else_if_block");

    // mettre ici les valeurs des expressions
    elseIfBB->add_IRInstr(new IRInstrIf(elseIfBB, "%ebx", "%eax",  "==",elseIfBB->getLabel()));
    this->currentCFG->add_bb(elseIfBB);
    this->currentCFG->setCurrentBasicBlock(elseIfBB);

    return 0;
}

antlrcpp::Any IRVisitor::visitElse_if_stmt_block(ifccParser::Else_if_stmt_blockContext *ctx)
{
    BasicBlock *elseIfBB = this->currentCFG->getCurrentBasicBlock();

    elseIfBB->add_IRInstr(new IRAfterIf(elseIfBB, elseIfBB->getLabel()));

    for (size_t i = 0; i < ctx->statement().size(); i++) {
        visit(ctx->statement(i));
    }
    if (ctx->return_stmt()) {
        visit(ctx->return_stmt());
    } else {
        elseIfBB->add_IRInstr(new IRInstrJump(elseIfBB, "end_if"));

    }
    return 0;
}

antlrcpp::Any IRVisitor::visitElse_block(ifccParser::Else_blockContext *ctx)
{
    /*BasicBlock *ifBB = new BasicBlock(this->currentCFG, "else_block");
    this->currentCFG->getCurrentBasicBlock()->add_IRInstr(new IRAfterIf(this->currentCFG->getCurrentBasicBlock(), ifBB->getLabel()));

    for (size_t i = 0; i < ctx->statement().size(); i++)
    {
        visit(ctx->statement(i));
    }

    if (ctx->return_stmt())
    {
        visit(ctx->return_stmt());
    }else{
        this->currentCFG->getCurrentBasicBlock()->add_IRInstr(new IRInstrIf(this->currentCFG->getCurrentBasicBlock(), "", "end"));
    }*/
    BasicBlock *elseBB = this->currentCFG->getCurrentBasicBlock();

    elseBB->add_IRInstr(new IRAfterIf(elseBB, elseBB->getLabel()));
    for (size_t i = 0; i < ctx->statement().size(); i++) {
        visit(ctx->statement(i));
    }
    if (ctx->return_stmt()) {
        visit(ctx->return_stmt());
    } else {
        elseBB->add_IRInstr(new IRInstrJump(elseBB, "end_if"));
    }

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
        visit(ctx);
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
        const string newTmpVar = this->currentCFG->create_new_tempvar(Type::INT);

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
