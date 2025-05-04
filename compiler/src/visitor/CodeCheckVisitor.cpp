#include "CodeCheckVisitor.h"

CodeCheckVisitor::CodeCheckVisitor()
{
    this->root = new SymbolsTable(0);
    this->currentSymbolsTable = this->root;
}

CodeCheckVisitor::~CodeCheckVisitor()
{
    delete this->root;

    for (auto it = cfgs.begin(); it != cfgs.end(); ++it)
    {
        delete it->second;
    }
}

antlrcpp::Any CodeCheckVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    visitChildren(ctx);

    // Collect the usage of symbols from the root and all its children
    map<string, bool> symbolsUsage = collectSymbolsUsage(root);

    for (auto it = symbolsUsage.begin(); it != symbolsUsage.end(); ++it)
    {
        if (!it->second)
        {
            printWarning(ctx, "variable '" + it->first + "' declared but not used");
        }
    }

    if (functionsNumberOfParameters.find("main") == functionsNumberOfParameters.end())
    {
        printError(ctx, "function 'main' is missing");
        exit(1);
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    if (ctx->expr() != nullptr)
    {
        if (auto callCtx = dynamic_cast<ifccParser::Call_func_stmtContext *>(ctx->expr()))
        {
            if (currentSymbolsTable->getSymbolType(callCtx->VAR()->getText()) == Type::VOID)
            {
                printError(ctx, callCtx->VAR()->getText() + " is type void");
                exit(1);
            }
        }
        visitExpr(ctx->expr());
    }
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    if (ctx->TYPE()->getText() == "void")
    {
        printError(ctx, "cannot declare a variable of type 'void'");
        exit(1);
    }

    int exprIndex = 0;

    for (int i = 0; i < ctx->VAR().size(); ++i)
    {
        std::string varLeft = ctx->VAR(i)->getText();
        int arraySize = 1;

        // Verify if it's an array
        if (ctx->INTEGER(i) != nullptr)
        {
            arraySize = std::stoi(ctx->INTEGER(i)->getText());
            if (arraySize <= 0)
            {
                printError(ctx, "array '" + varLeft + "' size must be greater than 0");
                exit(1);
            }
        }

        if (currentSymbolsTable->containsSymbol(varLeft))
        {
            printError(ctx, "variable '" + varLeft + "' is already declared");
            exit(1);
        }

        int symbolSize = TypeManager::size_of(TypeManager::stringToType(ctx->TYPE()->getText())) * arraySize;
        this->currentOffset -= symbolSize;
        currentSymbolsTable->addSymbol(varLeft, TypeManager::stringToType(ctx->TYPE()->getText()), symbolSize);
        currentSymbolsTable->setSymbolUsage(varLeft, false);

        // If an expression is associated
        if (exprIndex < ctx->expr().size() && ctx->expr(exprIndex) != nullptr)
        {
            currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);

            ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);

            if (arraySize > 1)
            {
                auto tabInitCtx = dynamic_cast<ifccParser::Array_initContext *>(exprCtx);
                if (tabInitCtx)
                {
                    if (tabInitCtx->expr().size() > arraySize)
                    {
                        printError(ctx, "too many elements in array initialization for '" + varLeft + "'");
                        exit(1);
                    }
                }
                else
                {
                    printError(ctx, "invalid initialization for array '" + varLeft + "'");
                    exit(1);
                }
            }
            else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
            {
                std::string varRight = varCtx->getText();
                if (!currentSymbolsTable->containsSymbol(varRight))
                {
                    printError(ctx, "variable '" + varRight + "' is not declared");
                    exit(1);
                }
                else if (!currentSymbolsTable->symbolHasAValue(varRight))
                {
                    printWarning(ctx, "variable '" + varRight + "' is undefined");
                }

                currentSymbolsTable->setSymbolUsage(varRight, true);
                currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);
            }
            else if (auto callCtx = dynamic_cast<ifccParser::Call_func_stmtContext *>(ctx->expr(i)))
            {
                if (currentSymbolsTable->getSymbolType(callCtx->VAR()->getText()) == Type::VOID)
                {
                    printError(ctx, callCtx->VAR()->getText() + " is type void");
                    exit(1);
                }
            }
            else
            {
                visitExpr(exprCtx);
            }

            ++exprIndex;
        }
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    std::string varLeft = ctx->VAR()->getText();

    if (currentSymbolsTable->getSymbolIndex(varLeft) == 0)
    {
        printError(ctx, "variable '" + varLeft + "' used before declaration");
        exit(1);
    }

    int nbExpr = (ctx->expr(1) != nullptr) ? 2 : 1;

    currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);

    for (int i = 0; i < nbExpr; ++i)
    {
        if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr(i)))
        {
            std::string varRight = varCtx->getText();
            if (currentSymbolsTable->getSymbolIndex(varRight) == 0)
            {
                printError(ctx, "variable '" + varRight + "' is not declared");
                exit(1);
            }
            else if (!currentSymbolsTable->symbolHasAValue(varRight))
            {
                printWarning(ctx, "variable '" + varRight + "' is undefined");
            }

            currentSymbolsTable->setSymbolUsage(varRight, true);
        }
        else if (auto callCtx = dynamic_cast<ifccParser::Call_func_stmtContext *>(ctx->expr(i)))
        {
            if (currentSymbolsTable->getSymbolType(callCtx->VAR()->getText()) == Type::VOID)
            {
                printError(ctx, callCtx->VAR()->getText() + " is type void");
                exit(1);
            }
        }
        else
        {
            visitExpr(ctx->expr(i));
        }
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitVar(ifccParser::VarContext *ctx)
{
    std::string varName = ctx->VAR()->getText();

    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        printError(ctx, "variable '" + varName + "' used before declaration");
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        printWarning(ctx, "variable '" + varName + "' is undefined");
    }

    currentSymbolsTable->setSymbolUsage(varName, true);

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitExpr(ifccParser::ExprContext *expr)
{
    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        std::string varName = varCtx->VAR()->getText();

        if (currentSymbolsTable->getSymbolIndex(varName) == 0)
        {
            printError(expr, "variable '" + varName + "' used before declaration");
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varName))
        {
            printWarning(expr, "variable '" + varName + "' is undefined");
        }

        currentSymbolsTable->setSymbolUsage(varName, true);
    }
    else if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(expr))
    {
        std::string varName = assignCtx->VAR()->getText();

        if (currentSymbolsTable->getSymbolIndex(varName) == 0)
        {
            printError(expr, "variable '" + varName + "' used before declaration");
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varName))
        {
            printWarning(expr, "variable '" + varName + "' is undefined");
        }

        currentSymbolsTable->setSymbolDefinitionStatus(varName, true);

        // Handle nested assignment (like a = b = 3)
        visitExpr(assignCtx->expr());
    }
    else
    {
        visit(expr);
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitBitwise(ifccParser::BitwiseContext *ctx)
{

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitComp(ifccParser::CompContext *ctx)
{

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitUnary(ifccParser::UnaryContext *ctx)
{
    visitExpr(ctx->expr());
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost_stmt(ifccParser::Post_stmtContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        printError(ctx, "variable '" + varName + "' used before declaration");
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        printWarning(ctx, "variable '" + varName + "' is undefined");
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPre_stmt(ifccParser::Pre_stmtContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        printError(ctx, "variable '" + varName + "' used before declaration");
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        printWarning(ctx, "variable '" + varName + "' is undefined");
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPre(ifccParser::PreContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        printError(ctx, "variable '" + varName + "' used before declaration");
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        printWarning(ctx, "variable '" + varName + "' is undefined");
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost(ifccParser::PostContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        printError(ctx, "variable '" + varName + "' used before declaration");
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        printWarning(ctx, "variable '" + varName + "' is undefined");
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_func_stmt(ifccParser::Decl_func_stmtContext *ctx)
{
    string funcName = ctx->VAR(0)->getText();

    if (functionsNumberOfParameters.find(funcName) != functionsNumberOfParameters.end())
    {
        printError(ctx, "function '" + funcName + "' is already declared");
        exit(1);
    }

    if (ctx->VAR().size() > 6)
    {
        printError(ctx, "too many parameters for function '" + funcName + "' (max: 6)");
        exit(1);
    }

    functionsNumberOfParameters[funcName] = ctx->VAR().size() - 1;

    if (ctx->block())
    {
        bool hasReturnStmt = false;
        bool hasReturnValue = false;
        bool isVoidFunction = TypeManager::stringToType(ctx->TYPE(0)->getText()) == Type::VOID;

        for (auto stmt : ctx->block()->statement())
        {
            if (stmt->return_stmt())
            {
                hasReturnStmt = true;

                if (isVoidFunction && stmt->return_stmt()->expr())
                {
                    printError(ctx, "function '" + funcName + "' of type void cannot return a value");
                    exit(1);
                }

                if (!isVoidFunction && !stmt->return_stmt()->expr())
                {
                    printError(ctx, "function '" + funcName + "' of type " + ctx->TYPE(0)->getText() + " must return a value");
                    exit(1);
                }

                if (stmt->return_stmt()->expr())
                {
                    hasReturnValue = true;
                }
            }
        }

        if (!isVoidFunction && !hasReturnStmt)
        {
            printError(ctx, "function '" + funcName + "' must contain a return statement");
            exit(1);
        }

        SymbolsTable *newTable = new SymbolsTable(currentOffset - 4);
        newTable->setParent(root);
        root->addChild(newTable);

        for (int i = 1; i < ctx->VAR().size(); i++)
        {
            string paramName = ctx->VAR(i)->getText();
            int paramSize = TypeManager::size_of(TypeManager::stringToType(ctx->TYPE(i)->getText()));
            newTable->addSymbol(paramName, TypeManager::stringToType(ctx->TYPE(i)->getText()), paramSize, i);
            newTable->setSymbolUsage(paramName, false);
        }

        this->currentSymbolsTable = newTable;
        visit(ctx->block());
        this->currentSymbolsTable = this->currentSymbolsTable->getParent();

        cfgs[funcName] = new CFG(funcName, newTable, currentOffset - 4);
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx)
{
    string functionName = ctx->VAR()->getText();

    if (getFunctionNumberOfParameters(functionName) == -1)
    {
        printError(ctx, "function '" + functionName + "' used before declaration");
        exit(1);
    }

    if (functionsNumberOfParameters[functionName] != ctx->expr().size())
    {
        printError(ctx, "function '" + functionName + "' called with wrong number of parameters");
        exit(1);
    }

    if (ctx->expr().size() > 6)
    {
        printError(ctx, "too many parameters in function call to '" + functionName + "' (max: 6)");
        exit(1);
    }

    for (int i = 0; i < ctx->expr().size(); ++i)
    {
        visitExpr(ctx->expr(i));
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    SymbolsTable *newTable = new SymbolsTable(currentOffset);

    currentSymbolsTable->addChild(newTable);
    currentSymbolsTable = newTable;

    visitChildren(ctx);

    currentSymbolsTable = currentSymbolsTable->getParent();

    return 0;
}

int CodeCheckVisitor::getFunctionNumberOfParameters(string functionName)
{
    if (functionsNumberOfParameters.find(functionName) != functionsNumberOfParameters.end())
        return functionsNumberOfParameters[functionName];
    return -1;
}

map<string, bool> CodeCheckVisitor::collectSymbolsUsage(SymbolsTable *table)
{
    map<string, bool> symbolsUsage = table->getSymbolsUsage();

    for (auto child : table->getChildren())
    {
        auto childSymbolsUsage = collectSymbolsUsage(child);
        symbolsUsage.insert(childSymbolsUsage.begin(), childSymbolsUsage.end());
    }

    return symbolsUsage;
}

void CodeCheckVisitor::printError(antlr4::ParserRuleContext *ctx, const string &message)
{
    cerr << ctx->getStart()->getLine()
         << ":" << ctx->getStart()->getCharPositionInLine()
         << ": error: " << message << endl;
}

void CodeCheckVisitor::printWarning(antlr4::ParserRuleContext *ctx, const string &message)
{
    cerr << ctx->getStart()->getLine()
         << ":" << ctx->getStart()->getCharPositionInLine()
         << ": warning: " << message << endl;
}
