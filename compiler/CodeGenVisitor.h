#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>
#include <string>

using namespace std;

class CodeGenVisitor : public ifccBaseVisitor
{
public:
        CodeGenVisitor(map<string, int> symbolsTable);

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitConst(ifccParser::ConstContext *ctx) override;
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;
        virtual antlrcpp::Any visitPre(ifccParser::PreContext *ctx) override;
        virtual antlrcpp::Any visitPost(ifccParser::PostContext *ctx) override;

        // Nouveau visitor pour l'opérateur unaire '!'
        virtual antlrcpp::Any visitNot(ifccParser::NotContext *ctx) override;
        // opéraéteur unaire -
        virtual antlrcpp::Any visitNeg(ifccParser::NegContext *ctx) override;

        // Méthode d'aide pour visiter une expression (optionnelle)
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr, bool isFirst);

protected:
        map<string, int> symbolsTable;
};