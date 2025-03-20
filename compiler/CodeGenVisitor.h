#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>
#include <string>

using namespace std;

class CodeGenVisitor : public ifccBaseVisitor
{
public:
        CodeGenVisitor(map<string, int> symbolsTable, int currentOffset);
        void resetCurrentTemporaryOffset();
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr, bool isFirst);
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;
        virtual antlrcpp::Any visitBitBybit(ifccParser::BitBybitContext *ctx) override;
        virtual antlrcpp::Any visitNot(ifccParser::NotContext *ctx) override;
        virtual antlrcpp::Any visitComp(ifccParser::CompContext *ctx) override;
        virtual antlrcpp::Any visitNeg(ifccParser::NegContext *ctx) override;
        virtual std::any visitPre(ifccParser::PreContext *ctx) override;
        virtual std::any visitPost(ifccParser::PostContext *ctx) override;

protected:
        map<string, int> symbolsTable;
        int currentTemporaryOffset;
        int maxOffset;

private:
        void saveValueInStack(ifccParser::ExprContext *expr, string varName);
        void loadRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr);
};