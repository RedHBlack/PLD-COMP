#pragma once
#include "BasicBlock.h"
#include <ostream>
#include <string>
#include <map>
#include <vector>
#include "../utils/Type.h"
#include "../utils/SymbolsTable.h"

using namespace std;

class BasicBlock;

/**
 * @brief Represents a Control Flow Graph (CFG) in an Intermediate Representation (IR).
 *
 * A CFG consists of basic blocks and represents the flow of control in a program.
 * This class is responsible for managing the basic blocks and generating
 * assembly code corresponding to the control flow graph.
 */
class CFG
{
public:
    /**
     * @brief Constructs a CFG with the given label and symbol index information.
     *
     * Initializes the control flow graph with a label and symbol index, and sets the initial index
     * for the next free symbol.
     *
     * @param label The label for the CFG.
     * @param SymbolIndex A map that maps symbol names to their respective indices.
     * @param SymbolType A map that maps symbol names to their respective types.
     * @param initialNextFreeSymbolIndex The initial value for the next free symbol index.
     */
    CFG(string label, SymbolsTable *symbolsTable, int initialNextFreeSymbolIndex);

    /**
     * @brief Adds a basic block to the control flow graph.
     *
     * This method adds a new basic block to the vector of basic blocks that make up the CFG.
     *
     * @param bb A pointer to the basic block to add.
     */
    void add_bb(BasicBlock *bb);

    /**
     * @brief Generates the assembly code for the entire control flow graph.
     *
     * This method generates the assembly code for all basic blocks in the CFG.
     *
     * @param o The output stream where the assembly code will be written.
     */
    void gen_asm(ostream &o);

    /**
     * @brief Creates a new temporary variable with a unique name.
     *
     * This method generates a temporary variable of the specified type and returns its unique name.
     *
     * @param t The type of the new temporary variable.
     * @return The name of the newly created temporary variable.
     */
    string create_new_tempvar(Type t);

    /**
     * @brief Retrieves the index of a variable by its name.
     *
     * This method retrieves the index of the variable in the symbol table.
     *
     * @param name The name of the variable.
     * @return The index of the variable, or -1 if the variable does not exist.
     */
    int get_var_index(string name);

    /**
     * @brief Retrieves the type of a variable based on its name.
     *
     * This method looks up the variable's type from the symbols table using the provided variable name.
     *
     * @param name The name of the variable whose type is to be retrieved.
     * @return The type of the specified variable.
     */
    Type get_var_type(string name);

    /**
     * @brief Converts a variable name into a register name.
     *
     * This method converts the given variable name into a corresponding register name,
     * taking into account the position of the variable in the symbol table.
     *
     * @param name The name of the variable to convert.
     * @return The register name corresponding to the input variable.
     */
    string toRegister(string name)
    {
        const int index = this->get_var_index(name);

        static const vector<string> regParams = {"%edi", "%esi", "%edx", "%ecx", "%r8", "%r9"};

        if (index > 0)
        {
            return regParams[this->get_var_index(name) - 1];
        }

        if (name[0] == 't')
        {
            int index = -stoi(name.substr(3)); // Récupère la sous-chaîne après "tmp"
            return to_string(index) + "(%rbp)";
        }
        return to_string(index) + "(%rbp)";
    }

    /**
     * @brief Retrieves the current basic block in the control flow graph.
     *
     * This method returns a pointer to the current basic block being processed.
     *
     * @return A pointer to the current basic block.
     */
    BasicBlock *getCurrentBasicBlock();

    /**
     * @brief Sets the current basic block in the control flow graph.
     *
     * This method sets the basic block that is currently being processed in the CFG.
     *
     * @param bb A pointer to the basic block to set as the current block.
     */
    void setCurrentBasicBlock(BasicBlock *bb);

    /**
     * @brief Resets the next free symbol index to its initial value.
     *
     * This method resets the index for the next free symbol to its initial state.
     */
    void resetNextFreeSymbolIndex();

    string getBBName();

    /**
     * @brief Generates the Graphviz representation of the control flow graph (CFG).
     *
     * This function generates a Graphviz-compatible `.dot` file that visualizes the control flow of the program. The `.dot` file describes the nodes (representing basic blocks or instructions) and edges (representing the flow of control between the blocks) of the control flow graph.
     *
     * The generated Graphviz representation is written to the provided output stream.
     *
     * @param o The output stream to which the Graphviz `.dot` representation of the CFG is written. This is typically a file stream (e.g., `ofstream`) that writes to a `.dot` file.
     */
    void gen_cfg_graphviz(ostream &o);

    /**
     * @brief Retrieves the label associated with the control flow graph (CFG).
     *
     * This function returns a string label that represents the name or identifier associated with the current control flow graph. The label can be used to identify different parts of the program, such as functions or basic blocks.
     *
     * The label is typically used for naming the nodes and edges in the Graphviz `.dot` representation or for other program analysis purposes.
     *
     * @return A string representing the label of the control flow graph. This could be a function name, block identifier, or any other relevant label.
     */
    string getLabel();

    /**
     * @brief Sets the label for the control flow graph (CFG).
     *
     * This function sets a new label for the current control flow graph. The label can be used to identify different parts of the program, such as functions or basic blocks.
     *
     * @param label The new label to set for the control flow graph.
     */
    void setLabel(string label);

    /**
     * @brief Adds a while loop to the control flow graph.
     *
     * This function adds a while loop to the control flow graph by creating the necessary basic blocks for the test condition, loop body, and exit block.
     *
     * @param test The basic block representing the test condition of the while loop.
     * @param body The basic block representing the body of the while loop.
     * @param end_bb The basic block representing the exit point of the while loop.
     *
     *
     */
    void add_while(BasicBlock *test, BasicBlock *body, BasicBlock *end_bb);

    /**
     * @brief Adds an if-then-else structure to the control flow graph.
     *
     * This function adds an if-then-else structure to the control flow graph by creating the necessary basic blocks for the test condition, then block, else block, and end block.
     *
     * @param test The basic block representing the test condition of the if statement.
     * @param then_bb The basic block representing the "then" branch of the if statement.
     * @param else_bb The basic block representing the "else" branch of the if statement.
     * @param end_bb The basic block representing the exit point of the if-then-else structure.
     *
     *
     */
    void add_if_then_else(BasicBlock *test, BasicBlock *then_bb, BasicBlock *else_bb, BasicBlock *end_bb);

    /**
     *
     * Sets the symbols table for the control flow graph.
     *
     * @param symbolsTable A pointer to the symbols table to be set for the control flow graph.
     */
    void setSymbolsTable(SymbolsTable *symbolsTable);

    /**
     * Retrieves the symbols table associated with the control flow graph.
     *
     * @return A pointer to the symbols table associated with the control flow graph.
     */
    SymbolsTable *getSymbolsTable() { return symbolsTable; };

protected:
    /// The symbols table containing information about variables and their types.
    SymbolsTable *symbolsTable;

    /// The next available symbol index.
    int nextFreeSymbolIndex;

    /// The initial value for the next free symbol index.
    const int initialTempPos = 0;

    /// A vector containing all the basic blocks in the control flow graph.
    vector<BasicBlock *> bbs;

    /// A pointer to the current basic block being processed.
    BasicBlock *current_bb;

    /// The label associated with the control flow graph.
    string label;

    /// The ID of the basic block.
    int idBB;
};
