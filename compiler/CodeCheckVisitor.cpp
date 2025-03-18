#include "CodeCheckVisitor.h"
using namespace std;

antlrcpp::Any CodeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    for (int i = 0; i < ctx->VAR().size(); i++)
    {
        string varName = ctx->VAR(i)->getText();
        if (symbolsTable.find(varName) != symbolsTable.end())
        {
            cout << varName << " is already declared" << endl;
            exit(1);
        }

        currentOffset += 4;

        symbolsTable[varName] = currentOffset;
        isUsed[varName] = false;
    }

    return 0;
}

antlrcpp::Any CodeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    string varName = ctx->VAR()->getText();

    if (symbolsTable.find(varName) == symbolsTable.end())
    {
        cout << varName << " : use before declaration" << endl;
        exit(1);
    }

    isUsed[varName] = true;

    return 0;
}

map<string, int> CodeCheckVisitor::getSymbolsTable()
{
    return symbolsTable;
}

int CodeCheckVisitor::getCurrentOffset()
{
    return currentOffset;
}
