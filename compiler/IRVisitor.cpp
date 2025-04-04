#include "IR/Instr/IRInstrLoadConst.h"
#include "IR/Instr/IRInstrMove.h"
#include "IR/Instr/IRInstrArithmeticOp.h"
#include "IR/Instr/IRInstrUnaryOp.h"
#include "IR/Instr/IRInstrComp.h"
#include "IR/Instr/IRInstrClean.h"
#include "IR/Instr/IRInstrCall.h"
#include "IR/Instr/IRInstrStoreToArray.h"
#include "IR/Instr/IRInstrLoadFromArray.h"
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
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(exprCtx))
    {
        // Si on a une constante comme index
        if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(tabCtx->expr()))
        {
            string value = constCtx->getText();
            if (value.front() == '\'' && value.back() == '\'')
            {
                int asciiValue = static_cast<int>(value[1]);
                currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", asciiValue));
            }
            else
            {
                currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", stoi(value)));
            }
        }
        else
        {
            visitExpr(tabCtx->expr(), true);
            currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
        }
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
                arraySize = stoi(ctx->CONST(exprIndex)->getText());
            }
            // Vérifier si le token suivant est "="
            if (i + 1 < ctx->children.size())
            {
                antlr4::tree::TerminalNode *nextNode = nullptr;
                antlr4::tree::TerminalNode *nextNodeForArray = nullptr;
                if ((i + 1 < ctx->children.size()) && (nextNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 1])) && nextNode->getText() == "=" && !isArray)
                {
                    // La variable a bien un initialiseur
                    assignValueToVar(ctx->expr(exprIndex), varName);
                    exprIndex++;
                }
                else if ((i + 4 < ctx->children.size()) && (nextNodeForArray = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i + 4])) && nextNodeForArray->getText() == "=" && isArray)
                {
                    // On a une initialisation de tableau
                    auto arrayInit = dynamic_cast<ifccParser::Array_initContext *>(ctx->expr(exprIndex));
                    for (int j = 0; j < arraySize; j++)
                    {
                        // On va utiliser storeValueToArray pour stocker la valeur dans le tableau
                        // On calcule l'offset du tableau
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
    // On vérifie si on assigne à un tableau ou à une variable simple
    if (ctx->expr(1) != nullptr)
    {
        // On a un tableau
        string arrayName = ctx->VAR()->getText();
        assignValueToArray(arrayName, ctx->expr(0), ctx->expr(1));
    }
    else
    {
        assignValueToVar(ctx->expr(0), ctx->VAR()->getText());
    }
    return 0;
}

void IRVisitor::assignValueToArray(string arrayName, ifccParser::ExprContext *indexExpr, ifccParser::ExprContext *valueExpr)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    // Évaluer l'expression de la valeur à affecter, résultat dans %ebx (ou eax si tableau).
    visitExpr(valueExpr, false);

    // Si en rvalue j'ai un tableau, je déplace d'abord eax dans ebx
    if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(valueExpr))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", "%ebx"));
    }

    // Cas de l'index constant
    if (auto constIndex = dynamic_cast<ifccParser::ConstContext *>(indexExpr))
    {
        int idx = stoi(constIndex->CONST()->getText());
        // Calculer l'offset complet pour ce tableau
        int computedOffset = this->currentCFG->get_var_index(arrayName) + idx * 4;
        // Utiliser IRInstrStoreToArray avec un indexRegister vide pour signaler que l'index est constant

        currentBB->add_IRInstr(new IRInstrStoreToArray(currentBB, computedOffset, "", "%ebx"));
    }
    else
    {
        // Cas dynamique : on évalue l'index dans %eax, on sauvegarde le résultat, on effectue cltq, etc.
        visitExpr(indexExpr, true);

        currentBB->add_IRInstr(new IRInstrUnaryOp(currentBB, "%eax", "cltq"));
        int baseOffset = this->currentCFG->get_var_index(arrayName);
        currentBB->add_IRInstr(new IRInstrStoreToArray(currentBB, baseOffset, "%rax", "%ebx"));
    }
}

void IRVisitor::loadValueFromArray(string arrayName, ifccParser::ExprContext *indexExpr, string targetRegister)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();
    if (auto constIndex = dynamic_cast<ifccParser::ConstContext *>(indexExpr))
    {
        int idx = stoi(constIndex->CONST()->getText());
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, arrayName, targetRegister, idx));
    }
    else
    {
        visitExpr(indexExpr, true);
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, arrayName, targetRegister, -1));
    }
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

antlrcpp::Any IRVisitor::visitArray_access(ifccParser::Array_accessContext *ctx)
{
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    if (auto innerAssign = dynamic_cast<ifccParser::Array_accessContext *>(ctx->expr()))
    {
        visitArray_access(innerAssign);
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
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(expr))
    {
        visitExpr(tabCtx->expr(), true);
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
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

antlrcpp::Any IRVisitor::visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx)
{
    static const vector<string> regParams = {"%edi", "%esi", "%edx", "%ecx", "%r8", "%r9"};
    BasicBlock *currentBB = this->currentCFG->getCurrentBasicBlock();

    for (int i = 0; i < ctx->expr().size(); ++i)
    {
        if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx->expr(i)))
        {

            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(constCtx->CONST()->getText()), regParams[i]));
        }
        else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr(i)))
        {
            currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), regParams[i]));
        }
        else
        {
            visitExpr(ctx->expr(i), true);
            currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", regParams[i]));
        }
    }

    currentBB->add_IRInstr(new IRInstrCall(currentBB, ctx->VAR()->getText()));

    return 0;
}

std::optional<int> IRVisitor::evaluateConstantExpression(ifccParser::ExprContext *ctx)
{
    // Case of a constant
    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        string value = constCtx->CONST()->getText();
        if (value.front() == '\'' && value.back() == '\'')
        {
            return static_cast<int>(value[1]);
        }
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
    else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx))
    {
        currentBB->add_IRInstr(new IRInstrMove(currentBB, varCtx->VAR()->getText(), varName));
    }
    else if (auto tabCtx = dynamic_cast<ifccParser::Array_accessContext *>(ctx))
    {
        visitExpr(tabCtx->expr(), true);
        currentBB->add_IRInstr(new IRInstrLoadFromArray(currentBB, tabCtx->VAR()->getText(), "%eax", -1));
        currentBB->add_IRInstr(new IRInstrMove(currentBB, "%eax", varName));
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

        string value = constCtx->CONST()->getText();
        if (value.front() == '\'' && value.back() == '\'')
        {
            int asciiValue = static_cast<int>(value[1]);
            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, asciiValue, "%eax"));
        }
        else
        {
            currentBB->add_IRInstr(new IRInstrLoadConst(currentBB, stoi(value), "%eax"));
        }
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