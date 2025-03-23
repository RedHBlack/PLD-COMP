#pragma once
#include "BasicBlock.h"
#include <ostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

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
     * @param initialNextFreeSymbolIndex The initial value for the next free symbol index.
     */
    CFG(string label, map<string, int> SymbolIndex, int initialNextFreeSymbolIndex);

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
     * @brief Generates the assembly prologue for the control flow graph.
     *
     * This method generates the initial assembly code that sets up the environment for the program
     * to start execution.
     *
     * @param o The output stream where the prologue will be written.
     */
    void gen_asm_prologue(ostream &o);

    /**
     * @brief Generates the assembly epilogue for the control flow graph.
     *
     * This method generates the final assembly code that cleans up the environment after the program
     * has executed.
     *
     * @param o The output stream where the epilogue will be written.
     */
    void gen_asm_epilogue(ostream &o);

    /**
     * @brief Creates a new temporary variable in the control flow graph.
     *
     * This method creates a new temporary variable with a unique name and returns the name of the variable.
     *
     * @return The name of the new temporary variable.
     */
    string create_new_tempvar();

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

protected:
    /// A map of symbol names to their respective indices.
    map<string, int> SymbolIndex;

    /// The next available symbol index.
    int nextFreeSymbolIndex;

    /// The initial value for the next free symbol index.
    const int initialTempPos;

    /// A vector containing all the basic blocks in the control flow graph.
    vector<BasicBlock *> bbs;

    /// A pointer to the current basic block being processed.
    BasicBlock *current_bb;

    /// The label associated with the control flow graph.
    string label;
};
