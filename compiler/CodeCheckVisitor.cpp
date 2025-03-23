#include "CodeCheckVisitor.h"

antlrcpp::Any CodeCheckVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visitExpr(ctx->expr());
    return 0;
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
                    cout << "#ERROR : The variable " << varRight << " is not declared." << endl;
                    exit(1);
                }
                else if (hasAValue.find(varRight) == hasAValue.end())
                {
                    cout << "#WARNING : The variable " << varRight << " is not initialized." << endl;
                }
                isUsed[varRight] = true;
                hasAValue[varLeft] = true;
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
        cout << "#ERROR: " << varLeft << " : use before declaration" << endl;
        exit(1);
    }

    auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr());
    if (varCtx != nullptr)
    {
        string varRight = varCtx->getText();
        if (symbolsTable.find(varRight) == symbolsTable.end())
        {
            cout << "#ERROR : The variable " << varRight << " is not declared." << endl;
            exit(1);
        }
        else if (hasAValue.find(varRight) == hasAValue.end())
        {
            cout << "#WARNING : The variable " << varRight << " is not initialized." << endl;
        }
        hasAValue[varLeft] = true;
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
        if (symbolsTable.find(varName) == symbolsTable.end())
        {
            cout << "#ERROR: " << varName << " : use before declaration" << endl;
            exit(1);
        }
        else if (hasAValue.find(varName) == hasAValue.end())
        {
            cout << "#WARNING : The variable " << varName << " is not initialized." << endl;
        }
        else
        {
            isUsed[varName] = true;
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
    if (symbolsTable.find(varName) == symbolsTable.end())
    {
        cout << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (hasAValue.find(varName) == hasAValue.end())
    {
        cout << "#WARNING : The variable " << varName << " is not initialized." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost(ifccParser::PostContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (symbolsTable.find(varName) == symbolsTable.end())
    {
        cout << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (hasAValue.find(varName) == hasAValue.end())
    {
        cout << "#WARNING : The variable " << varName << " is not initialized." << endl;
    }
    return 0;
}
