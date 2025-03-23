#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>

using namespace std;

class CodeCheckVisitor : public ifccBaseVisitor
{
public:
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr);
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;
        virtual antlrcpp::Any visitBitwise(ifccParser::BitwiseContext *ctx) override;
        virtual antlrcpp::Any visitComp(ifccParser::CompContext *ctx) override;
        virtual antlrcpp::Any visitUnary(ifccParser::UnaryContext *ctx) override;
        virtual antlrcpp::Any visitPre(ifccParser::PreContext *ctx) override;
        virtual antlrcpp::Any visitPost(ifccParser::PostContext *ctx) override;
        map<string, int> getSymbolsTable() const
        {
                return symbolsTable;
        }
        map<string, bool> getIsUsed() const { return isUsed; }
        int getCurrentOffset() const { return currentOffset; }

protected:
        map<string, int> symbolsTable;
        map<string, bool> isUsed;
        map<string, bool> hasAValue;
        int currentOffset = 0;
};