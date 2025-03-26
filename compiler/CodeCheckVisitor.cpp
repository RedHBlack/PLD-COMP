#include "CodeCheckVisitor.h"

CodeCheckVisitor::CodeCheckVisitor()
{
    this->root = new SymbolsTable(-4);
    this->currentSymbolsTable = this->root;
}

antlrcpp::Any CodeCheckVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    visitChildren(ctx);

    map<string, bool> symbolsUsage = root->getSymbolsUsage();

    for (auto it = symbolsUsage.begin(); it != symbolsUsage.end(); it++)
    {
        if (!it->second)
        {
            cout << "# WARNING: " << it->first << " : declared but not used" << endl;
        }
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;

    for (int i = 0; i < ctx->VAR().size(); i++)
    {
        string varLeft = ctx->VAR(i)->getText();
        if (currentSymbolsTable->containsSymbol(varLeft))
        {
            cout << "#ERROR: " << varLeft << " is already declared" << endl;
            exit(1);
        }

        currentSymbolsTable->addSymbol(varLeft, stringToType(ctx->TYPE()->getText()));
        currentSymbolsTable->setSymbolUsage(varLeft, false);

        currentOffset -= 4;

        // Seulement si l'expression existe pour cette variable
        if (exprIndex < ctx->expr().size() && ctx->expr(exprIndex) != nullptr)
        {
            ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);
            // Si l'initialiseur est une variable, on la marque comme utilisée
            if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
            {
                string varRight = varCtx->getText();
                if (currentSymbolsTable->getSymbolIndex(varRight) == 0)
                {
                    cout << "#ERROR : The variable " << varRight << " is not declared." << endl;
                    exit(1);
                }
                else if (!currentSymbolsTable->symbolHasAValue(varRight))
                {
                    cout << "#WARNING : The variable " << varRight << " is undefined." << endl;
                }
                currentSymbolsTable->setSymbolUsage(varRight, true);
                currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);
            }
            // Sinon, on visite l'expression pour marquer les variables utilisées
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
        cout << "#ERROR: " << varLeft << " : use before declaration" << endl;
        exit(1);
    }

    auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr());
    if (varCtx != nullptr)
    {
        string varRight = varCtx->getText();
        if (currentSymbolsTable->getSymbolIndex(varRight) == 0)
        {
            cout << "#ERROR : The variable " << varRight << " is not declared." << endl;
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varRight))
        {
            cout << "#WARNING : The variable " << varRight << " is undefined." << endl;
        }

        currentSymbolsTable->setSymbolUsage(varRight, true);
        currentSymbolsTable->setSymbolDefinitionStatus(varLeft, true);
    }
    else
    {
        visitExpr(ctx->expr());
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitExpr(ifccParser::ExprContext *expr)
{

    if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(expr))
    {
        string varName = varCtx->VAR()->getText();
        if (currentSymbolsTable->getSymbolIndex(varName) == 0)
        {
            cout << "#ERROR: " << varName << " : use before declaration" << endl;
            exit(1);
        }
        else if (!currentSymbolsTable->symbolHasAValue(varName))
        {
            cout << "#WARNING : The variable " << varName << " is undefined." << endl;
        }
        currentSymbolsTable->setSymbolUsage(varName, true);
    }
    else
    {
        visit(expr);
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAddsub(ifccParser::AddsubContext *ctx)
{
    // Vérification dans une expression arithmétique

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitMuldiv(ifccParser::MuldivContext *ctx)
{
    // Vérification dans une expression de multiplication ou division

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitBitwise(ifccParser::BitwiseContext *ctx)
{
    // Vérification dans une expression bit à bit

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitComp(ifccParser::CompContext *ctx)
{
    // Vérification dans une expression de comparaison

    visitExpr(ctx->expr(0));
    visitExpr(ctx->expr(1));

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitUnary(ifccParser::UnaryContext *ctx)
{
    visitExpr(ctx->expr());
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPre(ifccParser::PreContext *ctx)
{
    // Vérification dans une expression préfixée
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cout << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cout << "#WARNING : The variable " << varName << " is undefined." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost(ifccParser::PostContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (currentSymbolsTable->getSymbolIndex(varName) == 0)
    {
        cout << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (!currentSymbolsTable->symbolHasAValue(varName))
    {
        cout << "#WARNING : The variable " << varName << " is undefined." << endl;
    }
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    SymbolsTable *newTable = new SymbolsTable(currentOffset - 4);
    currentSymbolsTable->addChild(newTable);
    currentSymbolsTable = newTable;

    visit(ctx);

    currentSymbolsTable = currentSymbolsTable->getParent();

    return 0;
}
