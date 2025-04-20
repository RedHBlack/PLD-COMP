#include "BasicHelper.h"

optional<int> BasicHelper::evaluateConstantExpression(ifccParser::ExprContext *ctx)
{
    // Case of a constant
    if (auto constCtx = dynamic_cast<ifccParser::ConstContext *>(ctx))
    {
        int value = constCtx->cst()->CHAR() ? static_cast<int>(constCtx->cst()->CHAR()->getText()[1]) : stoi(constCtx->cst()->getText());
        return value;
    }

    // Case of an addition or subtraction
    if (auto addSubCtx = dynamic_cast<ifccParser::AddsubContext *>(ctx))
    {
        auto left = evaluateConstantExpression(addSubCtx->expr(0));
        auto right = evaluateConstantExpression(addSubCtx->expr(1));

        // Eliminate neutral elements
        if (right && *right == 0)
            return left; // x + 0 = x, x - 0 = x
        if (left && *left == 0 && addSubCtx->OP->getText() == "+")
            return right; // 0 + x = x

        if (left && right)
        {
            return addSubCtx->OP->getText() == "+" ? (*left + *right) : (*left - *right);
        }
    }

    // Case of a multiplication, division or modulo
    if (auto mulDivCtx = dynamic_cast<ifccParser::MuldivContext *>(ctx))
    {
        auto left = evaluateConstantExpression(mulDivCtx->expr(0));
        auto right = evaluateConstantExpression(mulDivCtx->expr(1));

        // Eliminate neutral elements
        if (right && *right == 1 && mulDivCtx->OP->getText() == "*")
            return left; // x * 1 = x
        if (left && *left == 1 && mulDivCtx->OP->getText() == "*")
            return right; // 1 * x = x
        if (right && *right == 1 && mulDivCtx->OP->getText() == "/")
            return left; // x / 1 = x

        if (left && right)
        {
            if (mulDivCtx->OP->getText() == "*")
                return (*left * *right);
            if (mulDivCtx->OP->getText() == "/")
                return (*left / *right);
            if (mulDivCtx->OP->getText() == "%")
                return (*right != 0 ? (*left % *right) : 0);
        }
    }

    // Case of a unary operation
    if (auto bitwiseCtx = dynamic_cast<ifccParser::BitwiseContext *>(ctx))
    {
        auto left = evaluateConstantExpression(bitwiseCtx->expr(0));
        auto right = evaluateConstantExpression(bitwiseCtx->expr(1));
        if (left && right)
        {
            if (bitwiseCtx->OP->getText() == "&")
                return (*left & *right);
            if (bitwiseCtx->OP->getText() == "|")
                return (*left | *right);
            if (bitwiseCtx->OP->getText() == "^")
                return (*left ^ *right);
        }
    }

    // Case of a comparison
    if (auto compCtx = dynamic_cast<ifccParser::CompContext *>(ctx))
    {
        auto left = evaluateConstantExpression(compCtx->expr(0));
        auto right = evaluateConstantExpression(compCtx->expr(1));
        if (left && right)
        {
            if (compCtx->OP->getText() == "==")
                return (*left == *right);
            if (compCtx->OP->getText() == "!=")
                return (*left != *right);
            if (compCtx->OP->getText() == "<")
                return (*left < *right);
            if (compCtx->OP->getText() == "<=")
                return (*left <= *right);
            if (compCtx->OP->getText() == ">")
                return (*left > *right);
            if (compCtx->OP->getText() == ">=")
                return (*left >= *right);
        }
    }

    // TODO : Handle other cases
    return nullopt; // If none of the above cases matched, return nullopt
}
