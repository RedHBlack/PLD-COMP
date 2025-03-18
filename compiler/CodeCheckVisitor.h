#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>
using namespace std;

class CodeCheckVisitor : public ifccBaseVisitor
{
public:
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        map<string, int> getSymbolsTable();

protected:
        map<string, int> symbolsTable;
        map<string, bool> isUsed;
        int currentOffset = 0;
};
