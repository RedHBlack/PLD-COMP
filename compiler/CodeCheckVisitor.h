#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "Type.h"
#include "SymbolsTable.h"
#include "IR/CFG.h"
#include <map>

using namespace std;

/**
 * @brief A visitor class for checking code correctness.
 *
 * This class extends the `ifccBaseVisitor` and is responsible for checking
 * various aspects of code correctness, including variable declarations, assignments,
 * and expressions, during the parsing phase of the compiler.
 */
class CodeCheckVisitor : public ifccBaseVisitor
{
public:
        /**
         * @brief Constructs a new instance of the CodeCheckVisitor.
         */
        CodeCheckVisitor();

        /**
         * @brief Visits the program context in the parsed code.
         *
         * This method performs a global check of the program, ensuring all variables
         * are declared and used properly.
         *
         * @param ctx The context for the program.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

        /**
         * @brief Visits an assignment statement in the parsed code.
         *
         * This method checks whether variables are assigned values correctly
         * and whether the variables involved are defined.
         *
         * @param ctx The context for the assignment statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;

        /**
         * @brief Visits a declaration statement in the parsed code.
         *
         * This method ensures that the variable being declared is correctly handled,
         * ensuring no variable is declared multiple times, or is used before being declared.
         *
         * @param ctx The context for the declaration statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;

        /**
         * @brief Visits any expression in the parsed code.
         *
         * This method processes expressions and checks whether variables in expressions
         * are valid and properly declared.
         *
         * @param expr The expression context to check.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitExpr(ifccParser::ExprContext *expr);

        /**
         * @brief Visits an addition or subtraction expression.
         *
         * This method processes expressions involving addition or subtraction
         * and checks for correctness in terms of variable usage.
         *
         * @param ctx The context for the addition or subtraction expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;

        /**
         * @brief Visits a multiplication or division expression.
         *
         * This method processes multiplication and division expressions
         * and checks whether variables involved are correctly declared.
         *
         * @param ctx The context for the multiplication or division expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;

        /**
         * @brief Visits a bitwise operation expression.
         *
         * This method processes bitwise operations like AND, OR, XOR, etc.,
         * and ensures that all variables in these operations are declared.
         *
         * @param ctx The context for the bitwise operation expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitBitwise(ifccParser::BitwiseContext *ctx) override;

        /**
         * @brief Visits a comparison expression.
         *
         * This method processes comparison operations like equality, inequality,
         * greater than, less than, etc., and checks for any correctness issues
         * regarding the variables used.
         *
         * @param ctx The context for the comparison expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitComp(ifccParser::CompContext *ctx) override;

        /**
         * @brief Visits a unary expression.
         *
         * This method processes unary expressions (e.g., negation or logical NOT)
         * and ensures that all variables in the expression are valid.
         *
         * @param ctx The context for the unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitUnary(ifccParser::UnaryContext *ctx) override;

        /**
         * @brief Visits a pre-unary operation (e.g., prefix increment/decrement).
         *
         * This method checks for correctness in expressions involving pre-unary operations.
         *
         * @param ctx The context for the pre-unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPre(ifccParser::PreContext *ctx) override;

        /**
         * @brief Visits a post-unary operation (e.g., postfix increment/decrement).
         *
         * This method processes post-unary operations and checks for correctness in usage.
         *
         * @param ctx The context for the post-unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPost(ifccParser::PostContext *ctx) override;

        /**
         * @brief Visits a function declaration statement.
         *
         * This method records information about the function being declared,
         * such as its parameter count.
         *
         * @param ctx The context for the function declaration statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitDecl_func_stmt(ifccParser::Decl_func_stmtContext *ctx) override;

        /**
         * @brief Visits a function call statement.
         *
         * This method checks whether the called function exists and matches the expected arguments.
         *
         * @param ctx The context for the function call statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx) override;

        /**
         * @brief Visits a block of code in the parsed code.
         *
         * This method ensures correct variable scoping and handles nested blocks properly.
         *
         * @param ctx The context for the block of code.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;

        /**
         * @brief Gets the current symbols table.
         *
         * @return The current symbols table.
         */
        SymbolsTable *getCurrentSymbolsTable() { return currentSymbolsTable; }

        /**
         * @brief Gets the root symbols table.
         *
         * @return The root symbols table.
         */
        SymbolsTable *getRootSymbolsTable() { return root; }

        int getCurrentOffset() { return currentOffset; }

        map<string, CFG *> getCFGS() { return cfgs; };

private:
        /**
         * @brief Gets the number of parameters for a function.
         *
         * This method retrieves the number of parameters for a given function name.
         * If the function is not found, it returns -1.
         * @param functionName The name of the function.
         * @return The number of parameters for the function, -1 if not found.
         */
        int getFunctionNumberOfParameters(string functionName);

        map<string, CFG *> cfgs;

        /// The root symbol table for the program.
        SymbolsTable *root;

        /// The current active symbol table.
        SymbolsTable *currentSymbolsTable;

        /// A map to track the number of parameters for each function.
        map<string, int> functionsNumberOfParameters;

        /// The current offset for variables.
        int currentOffset = 0;
};
