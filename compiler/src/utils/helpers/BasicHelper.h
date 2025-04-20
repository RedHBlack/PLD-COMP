#include "antlr4-runtime.h"
#include "../../generated/ifccParser.h"
#include <optional>

using namespace std;

/**
 * @brief Helper class for basic functionality related to parsing and evaluating expressions.
 *
 * This class contains methods for evaluating constant expressions and performing other basic tasks
 * related to parsing and evaluating expressions in the compiler.
 */
class BasicHelper
{
public:
    /**
     * @brief Evaluates a constant expression represented by an AST node.
     *
     * @param ctx An AST node representing the constant expression.
     * @return The evaluated integer value of the constant expression, or nullopt if it cannot be evaluated.
     */
    static optional<int> evaluateConstantExpression(ifccParser::ExprContext *ctx);
};