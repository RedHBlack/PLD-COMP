#include "IR/Instr/IRInstrLoadConst.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrArithmeticOp.h"
#include "IR/Instr/IRInstrUnaryOp.h"
#include "IR/Instr/IRInstrComp.h"
#include "IR/Instr/IRInstrClean.h"
#include "IR/Instr/IRInstrIf.h"
#include "IR/Instr/IRInstrJmpRet.h"
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
{   /*
    auto if_stmt = ctx->if_block();
    string cond = if_stmt->if_expr_block()->expr()->getText();
    visit(if_stmt->if_expr_block()->expr());

    string joinLabel = this->currentCFG->getBBName();
    BasicBlock *joinBB = new BasicBlock(this->currentCFG, joinLabel);
    joinBB->setIsTestVar(true);
    this->currentCFG->add_bb(joinBB);

    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    currentBB->setTestVarName(cond);

    string thenLabel = this->currentCFG->getBBName();
    BasicBlock *thenBB = new BasicBlock(this->currentCFG, thenLabel);
    thenBB->setIsTestVar(true);
    this->currentCFG->add_bb(thenBB);

    if (ctx->else_block())
    {
        string elseLabel = this->currentCFG->getBBName();
        BasicBlock *elseBB = new BasicBlock(this->currentCFG, elseLabel);
        elseBB->setIsTestVar(true);
        this->currentCFG->add_bb(elseBB);

        currentBB->setExitTrue(thenBB);
        currentBB->setExitFalse(elseBB);

        this->currentCFG->setCurrentBasicBlock(thenBB);
        visit(ctx->if_block()->if_stmt_block());
        thenBB->add_IRInstr(new IRInstrIf(thenBB, "%eax", joinLabel));

        this->currentCFG->setCurrentBasicBlock(elseBB);
        visit(ctx->else_block());
        elseBB->add_IRInstr(new IRInstrIf(elseBB, "%eax", joinLabel));
    } else {
        currentBB->setExitTrue(thenBB);
        currentBB->setExitFalse(joinBB);

        this->currentCFG->setCurrentBasicBlock(thenBB);
        visit(ctx->if_block()->if_stmt_block());
        thenBB->add_IRInstr(new IRInstrIf(thenBB, "%eax", joinLabel));
    }

    this->currentCFG->setCurrentBasicBlock(joinBB);
    */
    // Crear el bloque de prueba
    BasicBlock *testBB = this->currentCFG->getCurrentBasicBlock();
    testBB->setIsTestVar(true);

    // Linearizar la expresión de prueba
    visit(ctx->if_block()->if_expr_block()->expr());

    // Crear el bloque "then"
    string thenLabel = this->currentCFG->getBBName();
    BasicBlock *thenBB = new BasicBlock(this->currentCFG, thenLabel);
    thenBB->setIsTestVar(true);
    this->currentCFG->add_bb(thenBB);

    // Crear el bloque "else" si existe
    BasicBlock *elseBB = nullptr;
    if (ctx->else_block()) {
        string elseLabel = this->currentCFG->getBBName();
        elseBB = new BasicBlock(this->currentCFG, elseLabel);
        elseBB->setIsTestVar(true);
        this->currentCFG->add_bb(elseBB);
    }

    // Crear el bloque final "end_if"
    string endIfLabel = this->currentCFG->getBBName();
    BasicBlock *endIfBB = new BasicBlock(this->currentCFG, endIfLabel);

    this->currentCFG->add_bb(endIfBB);

    // Conectar el bloque de prueba con los bloques "then" y "else"
    testBB->setExitTrue(thenBB);
    testBB->setExitFalse(elseBB ? elseBB : endIfBB);

    // Visitar el bloque "then"
    this->currentCFG->setCurrentBasicBlock(thenBB);
    visit(ctx->if_block()->if_stmt_block());
    BasicBlock *thenLastBB = this->currentCFG->getCurrentBasicBlock();
    if(ctx->if_block()->if_stmt_block()->return_stmt()) {
        thenLastBB->setExitTrue(nullptr); // No hay salida condicional después de un retorno
        thenLastBB->add_IRInstr(new IRInstrJmpRet(thenLastBB, "output"));
    } else {
        thenLastBB->setExitTrue(endIfBB);
    }

    // Visitar el bloque "else" si existe
    if (elseBB) {
        this->currentCFG->setCurrentBasicBlock(elseBB);
        visit(ctx->else_block());
        BasicBlock *elseLastBB = this->currentCFG->getCurrentBasicBlock();
        //elseLastBB->setIsTestVar(true);
        if(ctx->else_block()->return_stmt()) {
            elseLastBB->setExitTrue(nullptr); // No hay salida condicional después de un retorno
            elseLastBB->add_IRInstr(new IRInstrJmpRet(elseLastBB, "output"));
        } else {
            elseLastBB->setExitTrue(endIfBB);
        }
    }

    this->currentCFG->setCurrentBasicBlock(endIfBB);
    // Establecer el bloque final como el bloque actual
    
    return 0;
}

antlrcpp::Any IRVisitor::visitIf_block(ifccParser::If_blockContext *ctx)
{
    visit(ctx->if_expr_block());

    visit(ctx->if_stmt_block());

    return 0;
}

antlrcpp::Any IRVisitor::visitIf_expr_block(ifccParser::If_expr_blockContext *ctx)
{   
    visit(ctx->expr());
    
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    currentBB->add_IRInstr(new IRInstrIf(currentBB, "%eax", "thenBB"));

    return 0;
}

antlrcpp::Any IRVisitor::visitIf_stmt_block(ifccParser::If_stmt_blockContext *ctx)
{
    BasicBlock *ifBB = this->currentCFG->getCurrentBasicBlock();

    // Agregar las instrucciones del bloque "then"
    for (size_t i = 0; i < ctx->statement().size(); i++) {
        visit(ctx->statement(i));
    }

    // Manejar el retorno si existe
    if (ctx->return_stmt()) {
        visit(ctx->return_stmt());
        ifBB->setExitTrue(nullptr); // No hay salida condicional después de un retorno
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitElse_block(ifccParser::Else_blockContext *ctx)
{
    BasicBlock *elseBB = this->currentCFG->getCurrentBasicBlock();

    // Agregar las instrucciones del bloque "else"
    for (size_t i = 0; i < ctx->statement().size(); i++) {
        visit(ctx->statement(i));
    }

    // Manejar el retorno si existe
    if (ctx->return_stmt()) {
        visit(ctx->return_stmt());
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
