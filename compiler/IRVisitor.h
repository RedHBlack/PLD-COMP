#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "IR/CFG.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class IRVisitor : public ifccBaseVisitor
{
public:
        IRVisitor(map<string, int> symbolsTable, int baseStackOffset);
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr, bool isFirst);
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;
        virtual antlrcpp::Any visitBitwise(ifccParser::BitwiseContext *ctx) override;
        virtual antlrcpp::Any visitComp(ifccParser::CompContext *ctx) override;
        virtual antlrcpp::Any visitUnary(ifccParser::UnaryContext *ctx) override;
        virtual antlrcpp::Any visitPre(ifccParser::PreContext *ctx) override;
        virtual antlrcpp::Any visitPost(ifccParser::PostContext *ctx) override;
        void gen_asm(ostream &o);
        void setCurrentCFG(CFG *currentCFG);
        CFG *getCurrentCFG();

protected:
        map<string, CFG *> cfgs;
        CFG *currentCFG;

private:
        void assignValueToVar(ifccParser::ExprContext *expr, string varName);
        void loadRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr);
        void handleArithmeticOp(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr, string op);
};