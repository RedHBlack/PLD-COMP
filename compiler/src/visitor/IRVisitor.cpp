#include "../IR/instr/IRInstrLoadConst.h"
#include "../IR/instr/IRInstrMove.h"
#include "../IR/instr/IRInstrArithmeticOp.h"
#include "../IR/instr/IRInstrUnaryOp.h"
#include "../IR/instr/IRInstrComp.h"
#include "../IR/instr/IRInstrClean.h"
#include "../IR/instr/IRInstrCall.h"
#include "../IR/instr/IRInstrStoreToArray.h"
#include "../IR/instr/IRInstrLoadFromArray.h"
#include "../IR/instr/IRInstrIf.h"
#include "../IR/instr/IRInstrJmpRet.h"
#include "../IR/instr/IRInstrJmpCond.h"
#include "../IR/instr/IRInstrSet.h"
#include "../IR/instr/IRInstrLogical.h"
#include "../utils/helpers/BasicHelper.h"
#include "IRVisitor.h"
#include <iostream>
#include <map>
#include <string>
#include "../IR/CFG.h"
#include "../IR/BasicBlock.h"

using namespace std;

IRVisitor::IRVisitor(map<string, CFG *> cfgs)
{
    this->cfgs = cfgs;
    this->currentCFG = cfgs.begin()->second;
    this->currentSymbolsTable = this->currentCFG->getSymbolsTable()->getParent();
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext *ctx)
{

    for (int i = 0; i < ctx->decl_func_stmt().size(); i++)
    {
        visit(ctx->decl_func_stmt(i));
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    if (childIndices.find(currentSymbolsTable) == childIndices.end())
    {
        childIndices[currentSymbolsTable] = 0;
    }

    int &childIndex = childIndices[currentSymbolsTable];
    setCurrentSymbolsTable(currentSymbolsTable->getChildren()[childIndex++]);

    for (int i = 0; i < ctx->statement().size(); i++)
    {
        visitChildren(ctx->statement(i));
        this->currentCFG->resetNextFreeSymbolIndex();
    }

    setCurrentSymbolsTable(currentSymbolsTable->getParent());

    return 0;
}

antlrcpp::Any IRVisitor::visitShift(ifccParser::ShiftContext *ctx)
{
    handleArithmeticOp(ctx->expr(0), ctx->expr(1), ctx->OP->getText());

    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    _returned = true;

    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    auto evaluatedValue = BasicHelper::evaluateConstantExpression(exprCtx);

    if (evaluatedValue)
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, "%eax"));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        // if the expression is a variable, we need to load its value
        currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(varCtx->VAR()->getText()), "%eax"));
    }
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(exprCtx))
    {
        // If we have a constant as index
        if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(tabCtx->expr()))
        {
            string value = constCtx->getText();
            currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", stoi(value)));
        }
        else
        {
            visitExpr(tabCtx->expr(), "%eax");
            currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
        }
    }
    else if (exprCtx)
    {
        // Else, we need to evaluate the expression
        visit(exprCtx);
    }

    currentBB->add_IRInstr(new IRInstrJmpRet(currentBB, "output_" + this->currentCFG->getLabel()));

    return 0;
}

antlrcpp::Any IRVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    int exprIndex = 0;

    for (int i = 0; i < ctx->children.size(); i++)
    {
        auto varNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i]);
        if (varNode && varNode->getSymbol()->getType() == ifccParser::VAR)
        {
            string varName = varNode->getText();
            bool isArray = (i + 1 < ctx->children.size() && ctx->children[i + 1]->getText() == "[");
            int arraySize = 1;
            if (isArray)
            {
                arraySize = stoi(ctx->INTEGER(exprIndex)->getText());
            }
            // Verify if the next token is "="
            if (i + 1 < ctx->children.size())
            {
                antlr4::tree::TerminalNode *nextNode = nullptr;
                antlr4::tree::TerminalNode *nextNodeForArray = nullptr;
                if ((i + 1 < ctx->children.size()) && (nextNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1])) && nextNode->getText() == "=" && !isArray)
                {
                    // The variable has an initializer
                    assignValueToVar(ctx->expr(exprIndex), varName);
                    exprIndex++;
                }
                else if ((i + 4 < ctx->children.size()) && (nextNodeForArray = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 4])) && nextNodeForArray->getText() == "=" && isArray)
                {
                    // We have an array initialization
                    auto arrayInit = dynamic_cast<ifccParser::Array_initContext *>(ctx->expr(exprIndex));
                    for (int j = 0; j < arraySize; j++)
                    {
                        // We will use storeValueToArray to store the value in the array
                        // We calculate the offset of the array
                        int computedOffset = this->currentCFG->get_var_index(varName) + j * 4;
                        int value = stoi(arrayInit->expr(j)->getText());

                        string result = to_string(computedOffset) + "(%rbp)";

                        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, value, result));
                    }
                    exprIndex++;
                }
            }
        }
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // We check if we are assigning to an array or a simple variable
    if (ctx->expr(1) != nullptr)
    {
        // We have an array
        string arrayName = ctx->VAR()->getText();
        assignValueToArray(arrayName, ctx->expr(0), ctx->expr(1));
    }
    else
    {
        assignValueToVar(ctx->expr(0), ctx->VAR()->getText());
    }
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
        visitExpr(ctx->expr(), "%eax");
    }

    string varName = ctx->VAR()->getText();
    currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(varName)));

    return 0;
}

antlrcpp::Any IRVisitor::visitArray_access(ifccParser::Array_accessContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    if (auto innerAssign = dynamic_cast<ifccParser::Array_accessContext *>(ctx->expr()))
    {
        visitArray_access(innerAssign);
    }
    else
    {
        visitExpr(ctx->expr(), "%eax");
    }

    string varName = ctx->VAR()->getText();
    currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(varName)));

    return 0;
}

antlrcpp::Any IRVisitor::visitExpr(ifccParser::ExprContext *expr, string targetRegister)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    // Try to evaluate the expression at compile-time
    auto evaluatedValue = BasicHelper::evaluateConstantExpression(expr);

    if (evaluatedValue)
    {
        // If it's a constant expression, load the computed value directly
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, targetRegister));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        // If it's a variable, move its value into the target register
        currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(varCtx->VAR()->getText()), targetRegister));
    }
    else if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(expr))
    {
        string varName = assignCtx->VAR()->getText();

        // Obtain the right value
        visitExpr(assignCtx->expr(), "%eax");

        // Move the value into the target register
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(varName)));
    }
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(expr))
    {
        visitExpr(tabCtx->expr(), "%eax");
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
    }
    else
    {
        // General case: visit the expression and move the result if necessary
        visit(expr);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", targetRegister));
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
    fillRegisters(ctx->expr(0), ctx->expr(1));

    currentBB->add_IRInstr(new IRInstrComp(currentBB, "%ebx", "%eax", op));

    return 0;
}

antlrcpp::Any IRVisitor::visitUnary(ifccParser::UnaryContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText();

    visitExpr(ctx->expr(), "%eax");

    currentBB->add_IRInstr(new IRInstrUnaryOp(currentBB, "%eax", op));

    return 0;
}

antlrcpp::Any IRVisitor::visitPre_stmt(ifccParser::Pre_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText(); // "++" or "--"
    const string varName = ctx->VAR()->getText();
    const string regVar = this->currentCFG->toRegister(varName);

    // Determine the operation symbol
    string opSymbol = (op == "++") ? "+" : "-";

    // Step 1 : load the variable into a register
    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", regVar, opSymbol));

    // Step 2 : load the value of the variable into another register
    currentBB->add_IRInstr(new IRInstrMove(currentBB, regVar, "%eax"));

    return 0;
}

antlrcpp::Any IRVisitor::visitPre(ifccParser::PreContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText(); // "++" or "--"
    const string varName = ctx->VAR()->getText();
    const string regVar = this->currentCFG->toRegister(varName);

    // Determine the operation symbol
    string opSymbol = (op == "++") ? "+" : "-";

    // Step 1 : load the variable into a register
    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", regVar, opSymbol));

    // Step 2 : load the value of the variable into another register
    currentBB->add_IRInstr(new IRInstrMove(currentBB, regVar, "%eax"));

    return 0;
}

antlrcpp::Any IRVisitor::visitPost_stmt(ifccParser::Post_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText(); // "++" or "--"
    const string varName = ctx->VAR()->getText();
    const string regVar = this->currentCFG->toRegister(varName); // register for the variable

    // Step 1 : load the variable into a register
    currentBB->add_IRInstr(new IRInstrMove(currentBB, regVar, "%eax"));

    // Step 2 : load the value of the variable into another register
    string opSymbol = (op == "++") ? "+" : "-";
    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", regVar, opSymbol));

    return 0;
}

antlrcpp::Any IRVisitor::visitPost(ifccParser::PostContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    const string op = ctx->OP->getText(); // "++" or "--"
    const string varName = ctx->VAR()->getText();
    const string regVar = this->currentCFG->toRegister(varName); // register for the variable

    // Step 1 : load the variable into a register
    currentBB->add_IRInstr(new IRInstrMove(currentBB, regVar, "%eax"));

    // Step 2 : load the value of the variable into another register
    string opSymbol = (op == "++") ? "+" : "-";
    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "$1", regVar, opSymbol));

    return 0;
}

antlrcpp::Any IRVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx)
{
    // Evaluation of the test expression
    BasicBlock *testBB = this->currentCFG->getCurrentBasicBlock();
    testBB->setIsTestVar(true);
    visit(ctx->if_block()->if_expr_block()->expr());

    // Creation of the then block
    string thenLabel = this->currentCFG->getBBName();
    BasicBlock *thenBB = new BasicBlock(this->currentCFG, thenLabel);
    thenBB->setIsTestVar(true);
    this->currentCFG->add_bb(thenBB);

    // Creation of the else block if it exists
    BasicBlock *elseBB = nullptr;
    if (ctx->else_block())
    {
        string elseLabel = this->currentCFG->getBBName();
        elseBB = new BasicBlock(this->currentCFG, elseLabel);
        elseBB->setIsTestVar(true);
        this->currentCFG->add_bb(elseBB);
    }

    // Create a merge block
    string mergeLabel = this->currentCFG->getBBName();
    BasicBlock *mergeBB = new BasicBlock(this->currentCFG, mergeLabel);
    this->currentCFG->add_bb(mergeBB);

    // Branches of the test
    testBB->setExitTrue(thenBB);
    testBB->setExitFalse(elseBB ? elseBB : mergeBB);

    bool prevReturned = _returned;

    // then branch processing
    _returned = false;
    this->currentCFG->setCurrentBasicBlock(thenBB);
    visit(ctx->if_block()->if_stmt_block());
    bool thenReturned = _returned;

    // If then does not return, link it to the merge block
    if (!thenReturned)
    {
        this->currentCFG->getCurrentBasicBlock()->setExitTrue(mergeBB);
    }

    // else branch processing, if it exists
    bool elseReturned = false;
    if (elseBB)
    {
        _returned = false;
        this->currentCFG->setCurrentBasicBlock(elseBB);
        visit(ctx->else_block());
        elseReturned = _returned;

        // If else does not return, link it to the merge block
        if (!elseReturned)
        {
            this->currentCFG->getCurrentBasicBlock()->setExitTrue(mergeBB);
        }
    }

    // Update the _returned flag
    _returned = prevReturned || thenReturned || elseReturned;

    // Place the current block on the merge block
    this->currentCFG->setCurrentBasicBlock(mergeBB);

    return 0;
}

antlrcpp::Any IRVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx)
{
    // Recover the block before the loop
    BasicBlock *preLoop = this->currentCFG->getCurrentBasicBlock();

    // Create the condition block
    string condLabel = this->currentCFG->getBBName();
    BasicBlock *condBB = new BasicBlock(this->currentCFG, condLabel);
    condBB->setIsTestVar(true);
    this->currentCFG->add_bb(condBB);
    preLoop->setExitTrue(condBB);

    // Generate the condition
    this->currentCFG->setCurrentBasicBlock(condBB);
    visit(ctx->while_expr_block()->expr());

    // Create the body and exit blocks
    string bodyLabel = this->currentCFG->getBBName();
    BasicBlock *bodyBB = new BasicBlock(this->currentCFG, bodyLabel);
    this->currentCFG->add_bb(bodyBB);

    string exitLabel = this->currentCFG->getBBName();
    BasicBlock *exitBB = new BasicBlock(this->currentCFG, exitLabel);
    this->currentCFG->add_bb(exitBB);

    // Branches of the condition
    condBB->setExitTrue(bodyBB);
    condBB->setExitFalse(exitBB);

    // Save the context before entering the loop
    bool prevReturned = _returned;
    bool prevInLoop = _inLoop;
    _inLoop = true;
    _returned = false;

    // Save the condition block for repetitions
    BasicBlock *savedCondBB = condBB;

    // Process the body of the loop
    this->currentCFG->setCurrentBasicBlock(bodyBB);
    visit(ctx->while_stmt_block());

    // If the body has not returned, add a branch to the condition
    if (!_returned)
    {
        BasicBlock *lastBodyBB = this->currentCFG->getCurrentBasicBlock();
        if (lastBodyBB)
        {
            lastBodyBB->setExitTrue(condBB);
        }
    }

    // Restore the context and set the exit block as the current block
    _inLoop = prevInLoop;
    this->currentCFG->setCurrentBasicBlock(exitBB);
    _returned = _returned || prevReturned;

    return 0;
}

antlrcpp::Any IRVisitor::visitDecl_func_stmt(ifccParser::Decl_func_stmtContext *ctx)
{

    if (ctx->block())
    {
        this->currentCFG = this->cfgs[ctx->VAR(0)->getText()];
        string currentCFGLabel = currentCFG->getLabel();

        BasicBlock *input = new BasicBlock(currentCFG, "input_" + currentCFGLabel);
        BasicBlock *body = new BasicBlock(currentCFG, "body_" + currentCFGLabel);

        input->add_IRInstr(new IRInstrSet(input));

        currentCFG->add_bb(input);

        input->setExitTrue(body);

        currentCFG->add_bb(body);

        setCurrentSymbolsTable(currentCFG->getSymbolsTable());

        visit(ctx->block());

        setCurrentSymbolsTable(currentSymbolsTable->getParent());

        string outputLabel = "output_" + currentCFGLabel;

        BasicBlock *output = new BasicBlock(currentCFG, outputLabel);
        output->add_IRInstr(new IRInstrClean(output));

        currentCFG->getCurrentBasicBlock()->setExitTrue(output);

        currentCFG->add_bb(output);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx)
{
    static const vector<string> regParams = {"%edi", "%esi", "%edx", "%ecx", "%r8", "%r9"};
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    vector<string> savedTemps;

    int numberOfArgs = ctx->expr().size();

    for (int i = 0; i < ctx->expr().size(); ++i)
    {
        if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(i)))
        {
            int value = constCtx->cst()->CHAR() ? static_cast<int>(constCtx->cst()->CHAR()->getText()[1]) : stoi(constCtx->cst()->getText());
            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, value, regParams[i]));
        }
        else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr(i)))
        {
            currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(varCtx->VAR()->getText()), regParams[i]));
        }
        else
        {
            visitExpr(ctx->expr(i), "%eax");
            currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", regParams[i]));
        }
        string tmp = this->currentCFG->create_new_tempvar(Type::INT);
        savedTemps.push_back(tmp);
        this->currentCFG->getCurrentBasicBlock()->add_IRInstr(
            new IRInstrMove(this->currentCFG->getCurrentBasicBlock(), regParams[i], this->currentCFG->toRegister(tmp)));
    }

    currentBB->add_IRInstr(new IRInstrCall(currentBB, ctx->VAR()->getText()));

    for (int r = 0; r < numberOfArgs; r++)
    {
        this->currentCFG->getCurrentBasicBlock()->add_IRInstr(
            new IRInstrMove(this->currentCFG->getCurrentBasicBlock(), this->currentCFG->toRegister(savedTemps[r]), regParams[r]));
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitLogicalAND(ifccParser::LogicalANDContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    fillRegisters(ctx->expr(0), ctx->expr(1));

    currentBB->add_IRInstr(new IRInstrLogical(currentBB, "%ebx", "%eax", "and"));

    return 0;
}

antlrcpp::Any IRVisitor::visitLogicalOR(ifccParser::LogicalORContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    fillRegisters(ctx->expr(0), ctx->expr(1));

    currentBB->add_IRInstr(new IRInstrLogical(currentBB, "%ebx", "%eax", "or"));

    return 0;
}

void IRVisitor::assignValueToVar(ifccParser::ExprContext *ctx, string varName)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    auto evaluatedValue = BasicHelper::evaluateConstantExpression(ctx);

    if (evaluatedValue)
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, this->currentCFG->toRegister(varName)));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(varCtx->VAR()->getText()), this->currentCFG->toRegister(varName)));
    }
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(ctx))
    {
        visitExpr(tabCtx->expr(), "%eax");
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(varName)));
    }
    else
    {
        // If the evaluated value is not a constant, we need to generate the code for the expression
        // and then assign the result to the variable
        visit(ctx);
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(varName)));
    }
}

void IRVisitor::assignValueToArray(string arrayName, ifccParser::ExprContext *indexExpr, ifccParser::ExprContext *valueExpr)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    // We evaluate the value expression, result in %ebx (or eax if it's an array).
    visitExpr(valueExpr, "%ebx");

    // If we have an array in rvalue, we first move eax to ebx
    if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(valueExpr))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", "%ebx"));
    }

    // Constant index case
    if (auto constIndex = dynamic_cast<ifccParser::ConstContext *>(indexExpr))
    {
        int idx = stoi(constIndex->cst()->getText());
        // Calculate the offset of the array
        int computedOffset = this->currentCFG->get_var_index(arrayName) + idx * 4;
        // Use IRInstrStoreToArray with an empty indexRegister to indicate that the index is constant

        currentBB->add_IRInstr(new IRInstrStoreToArray(currentBB, computedOffset, "", "%ebx"));
    }
    else
    {
        // Dynamic index case: we evaluate the index in %eax, save the result, perform cltq, etc.
        visitExpr(indexExpr, "%eax");

        // If it's an assignment in the index
        if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(indexExpr))
        {
            // We have to move the assigned variable to %eax
            currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(assignCtx->VAR()->getText()), "%eax"));
        }

        currentBB->add_IRInstr(new IRInstrUnaryOp(currentBB, "%eax", "cltq"));
        int baseOffset = this->currentCFG->get_var_index(arrayName);
        currentBB->add_IRInstr(new IRInstrStoreToArray(currentBB, baseOffset, "%rax", "%ebx"));
    }
}

void IRVisitor::fillRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(leftExpr))
    {
        visitExpr(rightExpr, "%ebx");

        string value = constCtx->cst()->getText();
        if (constCtx->cst()->CHAR())
        {
            int asciiValue = static_cast<int>(constCtx->cst()->CHAR()->getText()[1]);
            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, asciiValue, "%eax"));
        }
        else
        {
            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(value), "%eax"));
        }
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(leftExpr))
    {
        visitExpr(rightExpr, "%ebx");
        currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(varCtx->VAR()->getText()), "%eax"));
    }
    else
    {
        const string newTmpVar = this->currentCFG->create_new_tempvar(Type::INT);

        visitExpr(leftExpr, "%eax");
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", this->currentCFG->toRegister(newTmpVar)));
        visitExpr(rightExpr, "%ebx");
        currentBB->add_IRInstr(new IRInstrMove(currentBB, this->currentCFG->toRegister(newTmpVar), "%eax"));
    }
}

void IRVisitor::loadValueFromArray(string arrayName, ifccParser::ExprContext *indexExpr, string targetRegister)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    if (auto constIndex = dynamic_cast<ifccParser::ConstContext *>(indexExpr))
    {
        int idx = stoi(constIndex->cst()->getText());
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, arrayName, targetRegister, idx));
    }
    else
    {
        visitExpr(indexExpr, "%eax");
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, arrayName, targetRegister, -1));
    }
}

void IRVisitor::gen_asm(ostream &o)
{
    for (auto it = cfgs.begin(); it != cfgs.end(); ++it)
    {
        CFG *cfg = it->second;
        cfg->gen_asm(o);
    }
}

void IRVisitor::handleArithmeticOp(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr, string op)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    fillRegisters(leftExpr, rightExpr);

    currentBB->add_IRInstr(new IRInstrArithmeticOp(currentBB, "%ebx", "%eax", op));
}

map<string, CFG *> IRVisitor::getCFGS()
{
    return cfgs;
}

void IRVisitor::setCurrentSymbolsTable(SymbolsTable *currentSymbolsTable)
{
    this->currentSymbolsTable = currentSymbolsTable;
    this->currentCFG->setSymbolsTable(currentSymbolsTable);
}
