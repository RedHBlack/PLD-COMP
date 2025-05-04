#pragma once

#include "antlr4-runtime.h"
#include "../generated/ifccBaseVisitor.h"
#include "../utils/SymbolsTable.h"
#include "../IR/CFG.h"
#include <map>
#include <string>
#include <vector>
#include <optional>

using namespace std;

/**
 * @brief A visitor class for generating Intermediate Representation (IR) during parsing.
 *
 * This class extends the `ifccBaseVisitor` and is responsible for traversing the parsed code to
 * generate the Intermediate Representation (IR), such as the Control Flow Graph (CFG), for
 * the code being compiled.
 */
class IRVisitor : public ifccBaseVisitor
{
public:
        /**
         * @brief Constructs an IRVisitor.
         *
         * Initializes the IRVisitor with a symbols table and base stack offset.
         *
         * @param symbolsTable A map containing variable names and their associated stack offsets.
         * @param baseStackOffset The base offset for the stack.
         */
        IRVisitor(map<string, CFG *> cfgs);

        /**
         * @brief Visits the program and starts the IR generation process.
         *
         * This method starts the process of visiting the program node, generating the IR for the entire program.
         *
         * @param ctx The context of the program.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;

        /**
         * @brief Visits a block of code and generates the IR.
         *
         * This method processes blocks of code and generates the corresponding IR for each statement within the block.
         *
         * @param ctx The context of the block.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;

        /**
         * @brief Visits a return statement and generates the IR.
         *
         * This method processes return statements and generates the corresponding IR for the return operation.
         *
         * @param ctx The context of the return statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

        /**
         * @brief Visits an assignment statement and generates the IR.
         *
         * This method processes assignment statements and generates the corresponding IR for the variable assignment.
         *
         * @param ctx The context of the assignment statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;

        /**
         * @brief Visits an assignment expression in the parse tree.
         *
         * This method processes an assignment (`=`) in the input C code. It retrieves
         * the variable being assigned, evaluates the right-hand side expression, and
         * generates the necessary Intermediate Representation (IR) instructions.
         *
         * @param ctx The context of the assignment node in the parse tree.
         * @return The result of processing the assignment, wrapped in `antlrcpp::Any`.
         */
        virtual antlrcpp::Any visitAssign(ifccParser::AssignContext *ctx) override;

        /**
         * @brief Visits a declaration statement and generates the IR.
         *
         * This method processes declaration statements and generates the corresponding IR for the variable declaration.
         *
         * @param ctx The context of the declaration statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;

        /**
         * @brief Visits an array access expression and generates the IR.
         *
         * This method processes array access expressions and generates the corresponding IR for accessing elements in arrays.
         *
         * @param ctx The context of the array access expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitArray_access(ifccParser::Array_accessContext *ctx) override;

        /**
         * @brief Visits an expression and generates the IR.
         *
         * This method processes expressions and generates the corresponding IR for the expression.
         *
         * @param expr The expression context to generate IR for.
         * @param targetRegister The register to store the result of the expression evaluation.
         * @return A result of the visit, typically unused.
         */
        antlrcpp::Any visitExpr(ifccParser::ExprContext *expr, string targetRegister);

        /**
         * @brief Visits an addition or subtraction expression and generates the IR.
         *
         * This method processes addition and subtraction operations and generates the corresponding IR.
         *
         * @param ctx The context of the addition or subtraction expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitAddsub(ifccParser::AddsubContext *ctx) override;

        /**
         * @brief Visits a multiplication or division expression and generates the IR.
         *
         * This method processes multiplication and division operations and generates the corresponding IR.
         *
         * @param ctx The context of the multiplication or division expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitMuldiv(ifccParser::MuldivContext *ctx) override;

        /**
         * @brief Visits a bitwise operation expression and generates the IR.
         *
         * This method processes bitwise operations like AND, OR, XOR, etc., and generates the corresponding IR.
         *
         * @param ctx The context of the bitwise operation expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitBitwise(ifccParser::BitwiseContext *ctx) override;

        /**
         * @brief Visits a comparison expression and generates the IR.
         *
         * This method processes comparison operations (e.g., equality, greater-than) and generates the corresponding IR.
         *
         * @param ctx The context of the comparison expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitComp(ifccParser::CompContext *ctx) override;

        /**
         * @brief Visits a unary expression and generates the IR.
         *
         * This method processes unary operations (e.g., negation or logical NOT) and generates the corresponding IR.
         *
         * @param ctx The context of the unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitUnary(ifccParser::UnaryContext *ctx) override;

        /**
         * @brief Visits a shift operation (e.g., left or right shift) and generates the IR.
         *
         * This method processes shift operations and generates the corresponding IR.
         *
         * @param ctx The context of the shift operation.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitShift(ifccParser::ShiftContext *ctx) override;

        /**
         * @brief Visits a logical AND expression and generates the IR.
         *
         * This method processes logical AND operations and generates the corresponding IR.
         *
         * @param ctx The context of the logical AND expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitLogicalAND(ifccParser::LogicalANDContext *ctx) override;

        /**
         * @brief Visits a logical OR expression and generates the IR.
         *
         * This method processes logical OR operations and generates the corresponding IR.
         *
         * @param ctx The context of the logical OR expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitLogicalOR(ifccParser::LogicalORContext *ctx) override;

        /**
         * @brief Generates the assembly code for the IR.
         *
         * This method generates assembly code from the Intermediate Representation (IR) for output.
         *
         * @param o The output stream where the assembly code will be written.
         */
        void genASM(ostream &o);

        /**
         * @brief Visits a pre-unary operation (e.g., prefix increment/decrement) and generates the IR.
         *
         * This method processes pre-unary operations and generates the corresponding IR.
         *
         * @param ctx The context of the pre-unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPre(ifccParser::PreContext *ctx) override;

        /**
         * @brief Visits a pre-statement (e.g., prefix increment/decrement) and generates the IR.
         *
         * This method processes pre-statements and generates the corresponding IR.
         *
         * @param ctx The context of the pre-statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPre_stmt(ifccParser::Pre_stmtContext *ctx) override;

        /**
         * @brief Visits a post-unary operation (e.g., postfix increment/decrement) and generates the IR.
         *
         * This method processes post-unary operations and generates the corresponding IR.
         *
         * @param ctx The context of the post-unary expression.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPost(ifccParser::PostContext *ctx) override;

        /**
         * @brief Visits a post-statement (e.g., postfix increment/decrement) and generates the IR.
         *
         * This method processes post-statements and generates the corresponding IR.
         *
         * @param ctx The context of the post-statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitPost_stmt(ifccParser::Post_stmtContext *ctx) override;

        /**
         * @brief Visits a call function statement and generates the IR.
         *
         * This method processes calls to functions and generates the corresponding IR for calling those functions.
         *
         * @param ctx The context of the call function statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitDecl_func_stmt(ifccParser::Decl_func_stmtContext *ctx) override;

        /**
         * @brief Visits a call function statement and generates the IR.
         *
         * This method processes calls to functions and generates the corresponding IR for calling those functions.
         *
         * @param ctx The context of the call function statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitCall_func_stmt(ifccParser::Call_func_stmtContext *ctx) override;

        /**
         * @brief Visits an if statement and generates the IR.
         *
         * This method processes if statements and generates the corresponding IR for the conditional branching.
         *
         * @param ctx The context of the if statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitIf_stmt(ifccParser::If_stmtContext *ctx) override;

        /**
         * @brief Visits a while statement and generates the IR.
         *
         * This method processes while statements and generates the corresponding IR for the loop.
         *
         * @param ctx The context of the while statement.
         * @return A result of the visit, typically unused.
         */
        virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext *ctx) override;

        /**
         * @brief Retrieves the map of Control Flow Graphs (CFGs).
         *
         * This function returns a map where the keys are string labels (e.g., function names or block labels) and the values are pointers to the corresponding `CFG` objects. These `CFG` objects represent the control flow graphs of different sections of the parsed program.
         *
         * The returned map can be used for further analysis, visualization (e.g., by generating Graphviz `.dot` files), or manipulation of the program's control flow.
         *
         * @return A `std::map` where the key is a string label representing the section of the program (such as a function name) and the value is a pointer to the corresponding `CFG` object.
         */
        map<string, CFG *> getCFGS();

        /**
         * @brief Retrieves the current symbols table.
         * This method retrieves the current symbols table that is being used for the IR generation.
         * @return The current symbols table.
         */
        SymbolsTable *getCurrentSymbolsTable() { return currentSymbolsTable; }

        /**
         * @brief Sets the current symbols table.
         * This method sets the current symbols table that is being used for the IR generation.
         * @param currentSymbolsTable A pointer to the current symbols table.
         */
        void setCurrentSymbolsTable(SymbolsTable *currentSymbolsTable);

protected:
        /// A map of function names to their corresponding Control Flow Graphs (CFGs).
        map<string, CFG *> cfgs;

        /// A map of symbols tables to their corresponding indices in theirs scope.
        map<SymbolsTable *, int> childIndices;

        /// The current control flow graph (CFG) being used.
        CFG *currentCFG;

        /// The current symbols table being used.
        SymbolsTable *currentSymbolsTable;

private:
        /**
         * @brief Assigns a value to a variable.
         *
         * This method assigns a value to the given variable name by generating the corresponding IR.
         *
         * @param expr The expression that provides the value to assign.
         * @param varName The name of the variable to assign the value to.
         */
        void assignValueToVar(ifccParser::ExprContext *expr, string varName);

        /**
         * @brief Loads registers with values from expressions.
         *
         * This method loads values from expressions into registers for further processing.
         *
         * @param leftExpr The left expression in the assignment or operation.
         * @param rightExpr The right expression in the assignment or operation.
         */
        void fillRegisters(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr);

        /**
         * @brief Assigns a value to an array element.
         *
         * This method assigns a value to an array element at a specific index.
         *
         * @param arrayName The name of the array.
         * @param indexExpr The expression specifying the index of the array element to assign the value to.
         * @param valueExpr The expression providing the value to assign.
         */
        void assignValueToArray(string arrayName, ifccParser::ExprContext *indexExpr, ifccParser::ExprContext *valueExpr);

        /**
         * @brief Loads a value from an array element.
         *
         * This method loads a value from an array element specified by its index into a target register.
         *
         * @param arrayName The name of the array.
         * @param indexExpr The expression specifying the index of the array element to load.
         * @param targetRegister The register where the loaded value should be stored.
         */
        void loadValueFromArray(string arrayName, ifccParser::ExprContext *indexExpr, string targetRegister);

        /**
         * @brief Handles arithmetic operations (addition, subtraction, etc.).
         *
         * This method handles the generation of IR for arithmetic operations (e.g., addition, subtraction).
         *
         * @param leftExpr The left operand of the arithmetic operation.
         * @param rightExpr The right operand of the arithmetic operation.
         * @param op The operation to perform (e.g., "+" or "-").
         */
        void handleArithmeticOp(ifccParser::ExprContext *leftExpr, ifccParser::ExprContext *rightExpr, string op);

        /// A boolean flag indicating whether a return statement has occurred.
        bool _returned = false;

        /// A boolean flag indicating whether we are currently inside a loop.
        bool _inLoop = false;
};
