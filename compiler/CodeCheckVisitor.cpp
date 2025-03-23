#include "CodeCheckVisitor.h"

antlrcpp::Any CodeCheckVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    // Visite d'abord les enfants pour remplir isUsed
    antlrcpp::Any result = visitChildren(ctx);

    // We check if all the variables are used
    for (auto it = isUsed.begin(); it != isUsed.end(); it++)
    {
        if (!it->second)
        {
            cout << "# WARNING: " << it->first << " : declared but not used" << endl;
        }
    }

    return result;
}

antlrcpp::Any CodeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    int exprIndex = 0;
    for (int i = 0; i < ctx->VAR().size(); i++)
    {
        string varLeft = ctx->VAR(i)->getText();
        if (symbolsTable.find(varLeft) != symbolsTable.end())
        {
            cout << "#ERROR: " << varLeft << " is already declared" << endl;
            exit(1);
        }

        this->currentOffset -= 4;
        symbolsTable[varLeft] = currentOffset;
        isUsed[varLeft] = false;

        // Seulement si l'expression existe pour cette variable
        if (exprIndex < ctx->expr().size() && ctx->expr(exprIndex) != nullptr)
        {
            ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);
            // Si l'initialiseur est une variable, on la marque comme utilisée
            if (auto varCtx = dynamic_cast<ifccParser::VarContext *>(exprCtx))
            {
                string varRight = varCtx->getText();
                if (symbolsTable.find(varRight) == symbolsTable.end())
                {
                    cout << "#ERROR : The variable " << varRight << " is not declared." << std::endl;
                    exit(1);
                }
                isUsed[varRight] = true;
            }
            exprIndex++;
        }
    }
    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    string varLeft = ctx->VAR()->getText();

    if (symbolsTable.find(varLeft) == symbolsTable.end())
    {
        cout << "#WARNING: " << varLeft << " : use before declaration" << endl;
    }

    auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr());
    if (varCtx != nullptr)
    {
        string varRight = varCtx->getText();
        if (symbolsTable.find(varRight) == symbolsTable.end())
        {
            cout << "#WARNING : The variable " << varRight << " is not declared." << std::endl;
        }

        isUsed[varRight] = true;
    }

    return 0;
}
