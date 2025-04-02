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
            cerr << "# WARNING: " << it->first << " : declared but not used" << endl;
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
        int arraySize = 1; //Par défaut, une variable simple

        //Vérifie si c'est un tableau
        if (ctx->CONST(i) != nullptr)
        {
            arraySize = stoi(ctx->CONST(i)->getText()); //Taille du tableau
            if (arraySize <= 0)
            {
                cerr << "#ERROR: " << varLeft << " : array size must be greater than 0" << endl;
                exit(1);
            }
        }

        if (symbolsTable.find(varLeft) != symbolsTable.end())
        {
            cerr << "#ERROR: " << varLeft << " is already declared" << endl;
            exit(1);
        }

        this->currentOffset -= 4 * arraySize;
        symbolsTable[varLeft] = currentOffset;

        symbolsType[varLeft] = stringToType(ctx->TYPE()->getText());

        isUsed[varLeft] = false;

        // Seulement si l'expression existe pour cette variable/tableau
        if (exprIndex < ctx->expr().size() && ctx->expr(exprIndex) != nullptr)
        {
            ifccParser::ExprContext *exprCtx = ctx->expr(exprIndex);

            // Si l'initialiseur est une variable, on la marque comme utilisée
            
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
                if (symbolsTable.find(varRight) == symbolsTable.end())
                {
                    cerr << "#ERROR : The variable " << varRight << " is not declared." << endl;
                    exit(1);
                }
                else if (hasAValue.find(varRight) == hasAValue.end())
                {
                    cerr << "#WARNING : The variable " << varRight << " is not initialized." << endl;
                }
                isUsed[varRight] = true;
                hasAValue[varLeft] = true;
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

    if (symbolsTable.find(varLeft) == symbolsTable.end())
    {
        cerr << "#ERROR: " << varLeft << " : use before declaration" << endl;
        exit(1);
    }

    int nbExpr;
    if (ctx->expr(1) != nullptr) {
        nbExpr=2;
    }
    else {
        nbExpr=1;
    }
        
    for (int i=0 ; i<nbExpr; i++)
    {
        auto varCtx = dynamic_cast<ifccParser::VarContext *>(ctx->expr(i));
        if (varCtx != nullptr)
        {
            string varRight = varCtx->getText();
            if (symbolsTable.find(varRight) == symbolsTable.end())
            {
                cerr << "#ERROR : The variable " << varRight << " is not declared." << endl;
                exit(1);
            }
            else if (hasAValue.find(varRight) == hasAValue.end())
            {
                cerr << "#WARNING : The variable " << varRight << " is not initialized." << endl;
            }
            isUsed[varRight] = true;
            hasAValue[varLeft] = true;
        }
        else
        {
            visitExpr(ctx->expr(i));
        }
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
            cerr << "#ERROR: " << varName << " : use before declaration" << endl;
            exit(1);
        }
        else if (hasAValue.find(varName) == hasAValue.end())
        {
            cerr << "#WARNING : The variable " << varName << " is not initialized." << endl;
        }
        isUsed[varName] = true;
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
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (hasAValue.find(varName) == hasAValue.end())
    {
        cerr << "#WARNING : The variable " << varName << " is not initialized." << endl;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitPost(ifccParser::PostContext *ctx)
{
    string varName = ctx->VAR()->getText();
    if (symbolsTable.find(varName) == symbolsTable.end())
    {
        cerr << "#ERROR: " << varName << " : use before declaration" << endl;
        exit(1);
    }
    else if (hasAValue.find(varName) == hasAValue.end())
    {
        cerr << "#WARNING : The variable " << varName << " is not initialized." << endl;
    }
    return 0;
}
