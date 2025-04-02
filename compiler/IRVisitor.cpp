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

IRVisitor::IRVisitor(SymbolsTable *symbolsTable, int baseStackOffset)
{
    this->cfgs["main"] = new CFG("main", symbolsTable, baseStackOffset - 4);
    this->currentCFG = this->cfgs["main"];
    this->currentSymbolsTable = symbolsTable;
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    // for each function_stmt
    // name = function_stmt->getName()->geText()
    // this->cfgs[name] = new CFG(name, symbolsTable, baseStackOffset - 4);
    this->currentCFG->add_bb(new BasicBlock(this->currentCFG, "body"));

    for (int i = 0; i < ctx->statement().size(); i++)
    {
#ifdef __APPLE__
        int resultVisit = visitChildren(ctx->statement(i)).as<int>();
#else
        int resultVisit = any_cast<int>(visitChildren(ctx->statement(i)));
#endif
        if (resultVisit != 0)
            return 0;
        this->currentCFG->resetNextFreeSymbolIndex();
    }

    visit(ctx->return_stmt());

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

#ifdef __APPLE__
        int resultVisit = visitChildren(ctx->statement(i)).as<int>();
#else
        int resultVisit = any_cast<int>(visitChildren(ctx->statement(i)));
#endif
        if (resultVisit != 0)
            return 1;
    }

    if (ctx->return_stmt())
    {
        visit(ctx->return_stmt());
        return 1;
    }

    setCurrentSymbolsTable(currentSymbolsTable->getParent());

    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    ifccParser::ExprContext *exprCtx = ctx->expr();

    auto evaluatedValue = evaluateConstantExpression(exprCtx);

    if (evaluatedValue)
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, "%eax"));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
    {
        // if the expression is a variable, we need to load its value
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), "%eax"));
    }
    else
    {
        // Else, we need to evaluate the expression
        visit(exprCtx);
    }

    BasicBlock *output = new BasicBlock(this->currentCFG, "output");
    output->add_IRInstr(new IRInstrClean(output));

    this->currentCFG->getCurrentBasicBlock()->setExitTrue(output);
    this->currentCFG->add_bb(output);

    return 1;
}

antlrcpp::Any IRVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;

    for (int i = 0; i < ctx->children.size(); i++)
    {
        auto varNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i]);
        if (varNode && varNode->getSymbol()->getType() == ifccParser::VAR)
        {
            string varName = varNode->getText();
            if (i + 1 < ctx->children.size())
            {
                auto nextNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1]);
                if (nextNode && nextNode->getText() == "=")
                {
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
    std::string targetRegister = isFirst ? "%eax" : "%ebx";

    // Try to evaluate the expression at compile-time
    auto evaluatedValue = evaluateConstantExpression(expr);

    if (evaluatedValue)
    {
        // If it's a constant expression, load the computed value directly
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, targetRegister));
    }
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        // If it's a variable, move its value into the target register
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), targetRegister));
    }
    else if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(expr))
    {
        if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(assignCtx->VAR()))
        {
            // Process the right-hand side expression
            visitExpr(assignCtx->expr(), false);
            // Assign the result to the variable
            currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", assignCtx->VAR()->getText()));
        }
    }
    else
    {
        // General case: visit the expression and move the result if necessary
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

std::optional<int> IRVisitor::evaluateConstantExpression(ifccParser::ExprContext *ctx)
{
    // Case of a constant
    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        return stoi(constCtx->CONST()->getText());
    }

    // Case of an addition or subtraction
    if (auto addSubCtx = dynamic_cast<ifccParser::AddsubContext *>(ctx))
    {
        auto left = evaluateConstantExpression(addSubCtx->expr(0));
        auto right = evaluateConstantExpression(addSubCtx->expr(1));

        // Eliminate neutral elements
        if (right && *right == 0)
            return left; // x + 0 = x, x - 0 = x
        if (left && *left == 0 && addSubCtx->OP->getText() == "+")
            return right; // 0 + x = x

        if (left && right)
        {
            return addSubCtx->OP->getText() == "+" ? (*left + *right) : (*left - *right);
        }
    }

    // Case of a multiplication, division or modulo
    if (auto mulDivCtx = dynamic_cast<ifccParser::MuldivContext *>(ctx))
    {
        auto left = evaluateConstantExpression(mulDivCtx->expr(0));
        auto right = evaluateConstantExpression(mulDivCtx->expr(1));

        // Eliminate neutral elements
        if (right && *right == 1 && mulDivCtx->OP->getText() == "*")
            return left; // x * 1 = x
        if (left && *left == 1 && mulDivCtx->OP->getText() == "*")
            return right; // 1 * x = x
        if (right && *right == 1 && mulDivCtx->OP->getText() == "/")
            return left; // x / 1 = x

        if (left && right)
        {
            if (mulDivCtx->OP->getText() == "*")
                return (*left * *right);
            if (mulDivCtx->OP->getText() == "/")
                return (*left / *right);
            if (mulDivCtx->OP->getText() == "%")
                return (*right != 0 ? (*left % *right) : 0);
        }
    }

    // Case of a unary operation
    if (auto bitwiseCtx = dynamic_cast<ifccParser::BitwiseContext *>(ctx))
    {
        auto left = evaluateConstantExpression(bitwiseCtx->expr(0));
        auto right = evaluateConstantExpression(bitwiseCtx->expr(1));
        if (left && right)
        {
            if (bitwiseCtx->OP->getText() == "&")
                return (*left & *right);
            if (bitwiseCtx->OP->getText() == "|")
                return (*left | *right);
            if (bitwiseCtx->OP->getText() == "^")
                return (*left ^ *right);
        }
    }

    // Case of a comparison
    if (auto compCtx = dynamic_cast<ifccParser::CompContext *>(ctx))
    {
        auto left = evaluateConstantExpression(compCtx->expr(0));
        auto right = evaluateConstantExpression(compCtx->expr(1));
        if (left && right)
        {
            if (compCtx->OP->getText() == "==")
                return (*left == *right);
            if (compCtx->OP->getText() == "!=")
                return (*left != *right);
            if (compCtx->OP->getText() == "<")
                return (*left < *right);
            if (compCtx->OP->getText() == "<=")
                return (*left <= *right);
            if (compCtx->OP->getText() == ">")
                return (*left > *right);
            if (compCtx->OP->getText() == ">=")
                return (*left >= *right);
        }
    }

    // TODO : Handle other cases
    return std::nullopt; // If none of the above cases matched, return nullopt
}

void IRVisitor::assignValueToVar(ifccParser::ExprContext *ctx, string varName)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    auto evaluatedValue = evaluateConstantExpression(ctx);

    if (evaluatedValue)
    {
        currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, *evaluatedValue, varName));
    }
    else
    {
        // If the evaluated value is not a constant, we need to generate the code for the expression
        // and then assign the result to the variable
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

void IRVisitor::setCurrentSymbolsTable(SymbolsTable *currentSymbolsTable)
{
    this->currentSymbolsTable = currentSymbolsTable;
    this->currentCFG->setSymbolsTable(currentSymbolsTable);
}