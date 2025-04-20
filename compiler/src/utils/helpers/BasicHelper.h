#include "antlr4-runtime.h"
#include "../../generated/ifccParser.h"
#include <optional>

using namespace std;

class BasicHelper
{
public:
    static optional<int> evaluateConstantExpression(ifccParser::ExprContext *ctx);
};