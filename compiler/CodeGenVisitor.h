#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>

using namespace std;

class CodeGenVisitor : public ifccBaseVisitor
{
public:
        CodeGenVisitor(map<string, int> symbolsTable, int currentOffset);
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr, bool isFirst);
        virtual antlrcpp::Any visitConst(ifccParser::ConstContext *ctx) override;
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;
        virtual std::any visitPre(ifccParser::PreContext *ctx) override;
        virtual std::any visitPost(ifccParser::PostContext *ctx) override;
        virtual antlrcpp::Any visitBitBybit(ifccParser::BitBybitContext *ctx) override;
        // virtual std::any visitOpposite(ifccParser::OppositeContext *ctx) override;
        void resetCurrentTemporaryOffset();

protected:
        map<string, int> symbolsTable;
        int currentTemporaryOffset;
        int maxOffset;
};