#include "CodeCheckVisitor.h"

CodeCheckVisitor::CodeCheckVisitor()
{
    this->root = new SymbolsTable(0);
    this->currentSymbolsTable = this->root;
}

antlrcpp::Any CodeCheckVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    visitChildren(ctx);

    // Collecte l'utilisation des symboles depuis la racine et tous ses enfants
    map<string, bool> symbolsUsage = collectSymbolsUsage(root);

    for (auto it = symbolsUsage.begin(); it != symbolsUsage.end(); it++)
    {
        if (!it->second)
        {
            cerr << "# WARNING: " << it->first << " : declared but not used" << endl;
        }
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitReturn(ifccParser::Return_stmtContext *ctx)
{
    if (ctx->expr() != nullptr)
    {
        visitExpr(ctx->expr());
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;

    for (int i = 0; i < ctx->VAR().size(); i++)
    {
        string varLeft = ctx->VAR(i)->getText();
        int arraySize = 1; // Par défaut, une variable simple
        // Vérifie si c'est un tableau
        if (ctx->CONST(i) != nullptr)
        {
            arraySize = stoi(ctx->CONST(i)->getText()); // Taille du tableau
            if (arraySize <= 0)
            {
                cerr << "#ERROR: " << varLeft << " : array size must be greater than 0" << endl;
                exit(1);
            }
        }

        if (currentSymbolsTable->containsSymbol(varLeft))
        {
            cerr << "#ERROR: " << varLeft << " is already declared" << endl;
            exit(1);
        }
        int symbolSize = size_of(stringToType(ctx->TYPE()->getText())) * arraySize;
        this->currentOffset -= symbolSize;
        currentSymbolsTable->addSymbol(varLeft, stringToType(ctx->TYPE()->getText()), symbolSize);
        currentSymbolsTable->setSymbolUsage(varLeft, false);

        // Seulement si l'expression existe pour cette variable/tableau
        if (exprIndex < ctx->expr().size() && ctx->expr(exprIndex) != nullptr)
        {
            currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);

            ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);

            if (arraySize > 1)
            {
                if (auto tabInitCtx = dynamic_cast<ifccParser::Array_initContext *>(exprCtx))
                {
                    if (tabInitCtx->expr().size() > arraySize)
                    {
                        cerr << "#ERROR: Too many elements in array initialization for " << varLeft << endl;
                        exit(1);
                    }
                }
                else
                {
                    cerr << "#ERROR: Invalid initialization for array " << varLeft << endl;
                    exit(1);
                }
            }
            else if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
            {
                string varRight = varCtx->getText();
                if (!currentSymbolsTable->containsSymbol(varRight))
                {
                    cerr << "#ERROR : The variable " << varRight << " is not declared." << endl;
                    exit(1);
                }
                else if (!currentSymbolsTable->symbolHasAValue(varRight))
                {
                    cerr << "#WARNING : The variable " << varRight << " is undefined." << endl;
                }
                currentSymbolsTable->setSymbolUsage(varRight, true);
                currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);
            }
            else
            {
                visitExpr(exprCtx);
            }
            exprIndex++;
        }
    }
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    string varLeft = ctx->VAR()->getText();

    if (currentSymbolsTable->getSymbolIndex(varLeft) == 0)
    {
        cerr << "#ERROR: " << varLeft << " : use before declaration" << endl;
        exit(1);
    }

    int nbExpr;
    if (ctx->expr(1) != nullptr)
    {
        nbExpr = 2;
    }
    else
    {
        nbExpr = 1;
    }

    currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);

    for (int i = 0; i < nbExpr; i++)
    {
        auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr(i));
        if (varCtx != nullptr)
        {
            string varRight = varCtx->getText();
            if (currentSymbolsTable->getSymbolIndex(varRight) == 0)
            {
                cerr << "#ERROR : The variable " << varRight << " is not declared." << endl;
                exit(1);
            }
            else if (!currentSymbolsTable->symbolHasAValue(varRight))
            {
                cerr << "#WARNING : The variable " << varRight << " is undefined." << endl;
            }

            currentSymbolsTable->setSymbolUsage(varRight, true);
            currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);
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
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
    }

    currentSymbolsTable->setSymbolUsage(varName, true);

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitExpr(ifccParser::ExprContext *expr)
{
    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        string varName = varCtx->VAR()->getText();
        if (currentSymbolsTable->getSymbolIndex(varName) == 0)
        {
            cerr << "#ERROR: " << varName << " : use before declaration" << endl;
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varName))
        {
            cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
        }
        currentSymbolsTable->setSymbolUsage(varName, true);
    }
    if (auto assignCtx = dynamic_cast<ifccParser::AssignContext *>(expr))
    {
        string varName = assignCtx->VAR()->getText();
        currentSymbolsTable->setSymbolDefinitionStatus(varName, true);
        if (currentSymbolsTable->getSymbolIndex(varName) == 0)
        {
            cerr << "#ERROR: " << varName << " : use before declaration" << endl;
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varName))
        {
            cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
        }

        if (auto innerAssign = dynamic_cast<ifccParser::AssignContext *>(assignCtx->expr()))
        {
            visitExpr(innerAssign);
        }
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
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPre_stmt(ifccParser::Pre_stmtContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPre(ifccParser::PreContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost(ifccParser::PostContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cerr << "#WARNING : The variable " << varName << " is undefined." << endl;
    }
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_func_stmt(ifccParser::Decl_func_stmtContext *ctx)
{
    string funcName = ctx->VAR(0)->getText();

    // Check if the function is already declared
    if (functionsNumberOfParameters.find(funcName) != functionsNumberOfParameters.end())
    {
        cout << "#ERROR: " << funcName << " : redeclaration" << endl;
        exit(1);
    }

    // Check if the function has too many parameters
    if (ctx->VAR().size() > 6)
    {
        cout << "#ERROR: Too many parameters for function (max: 6)" << funcName << endl;
        exit(1);
    }

    // Add the function to the map with the number of parameters
    functionsNumberOfParameters[funcName] = ctx->VAR().size() - 1;

    // Check if the function has a block
    if (ctx->block())
    {
        // Create a new CFG for the function
        SymbolsTable *newTable = new SymbolsTable(currentOffset - 4);

        // Set the parent of the new table to the root
        newTable->setParent(root);

        // *** Ajout de newTable au root ***
        root->addChild(newTable);

        for (int i = 1; i < ctx->VAR().size(); i++)
        {
            string paramName = ctx->VAR(i)->getText();
            int paramSize = size_of(stringToType(ctx->TYPE(i)->getText()));
            newTable->addSymbol(paramName, stringToType(ctx->TYPE(i)->getText()), paramSize, i);
            newTable->setSymbolUsage(paramName, false);
        }

        this->currentSymbolsTable = newTable;

        visit(ctx->block());

        this->currentSymbolsTable = this->currentSymbolsTable->getParent();

        // Create a new CFG for the function
        cfgs[funcName] = new CFG(funcName, newTable, currentOffset - 4);
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx)
{
    string functionName = ctx->VAR()->getText();

    // Check if the function is declared
    if (getFunctionNumberOfParameters(functionName) == -1)
    {
        cout << "#ERROR: " << functionName << " : use before declaration" << endl;
        exit(1);
    }

    // Check if the number of arguments matches the function definition
    if (functionsNumberOfParameters[functionName] != ctx->expr().size())
    {
        cout << "#ERROR: " << functionName << " : wrong number of parameters" << endl;
        exit(1);
    }

    if (ctx->expr().size() > 6)
    {
        cout << "#ERROR: Too many parameters for function (max: 6) " << functionName << endl;
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

antlrcpp::Any CodeCheckVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx)
{
    visit(ctx->if_block());

    if (ctx->else_block())
    {
        visit(ctx->else_block());
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitIf_block(ifccParser::If_blockContext *ctx)
{
    visit(ctx->if_expr_block());

    visit(ctx->if_stmt_block());

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitIf_expr_block(ifccParser::If_expr_blockContext *ctx)
{
    visit(ctx->expr());

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx)
{
    visit(ctx->while_expr_block());

    visit(ctx->while_stmt_block());

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitWhile_expr_block(ifccParser::While_expr_blockContext *ctx)
{
    visit(ctx->expr());

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
