#pragma once

#include "CFG.h"
#include "instr/BaseIRInstr.h"
#include <vector>

using namespace std;

class CFG;
class BaseIRInstr;

/**
 * @brief Represents a basic block in the control flow graph (CFG).
 *
 * A basic block is a sequence of instructions with a single entry point and a single exit point.
 * This class is responsible for managing the instructions in the block and generating the assembly code.
 */
class BasicBlock
{
public:
  /**
   * @brief Constructs a BasicBlock with the given CFG and entry label.
   *
   * Initializes a new basic block with a label and associates it with a specific control flow graph (CFG).
   *
   * @param cfg The CFG where this basic block belongs.
   * @param entry_label The entry label for the basic block.
   */
  BasicBlock(CFG *cfg, string entry_label);

  /**
   * @brief Generates the assembly code for this basic block.
   *
   * This method generates the x86 assembly code for all the instructions in the basic block.
   *
   * @param o The output stream where the assembly code will be written.
   */
  void genASM(ostream &o);

  /**
   * @brief Adds an instruction to the basic block.
   *
   * This method adds a new intermediate representation instruction (IRInstr) to the basic block.
   *
   * @param instr A pointer to the instruction to add.
   */
  void addIRInstr(BaseIRInstr *instr);

  /**
   * @brief Gets the CFG associated with this basic block.
   *
   * This method retrieves the CFG to which this basic block belongs.
   *
   * @return A pointer to the CFG associated with this basic block.
   */
  CFG *getCFG();

  /**
   * @brief Retrieves the label associated with the current block.
   *
   * This function returns the label associated with this particular control flow block.
   * The label is typically used to uniquely identify this block in control flow analysis.
   *
   * @return A string representing the label of this control flow block.
   */
  string getLabel();

  /**
   * @brief Sets the label for the current block.
   *
   * This function sets a new label for this control flow block. The label is typically used
   * to uniquely identify this block in control flow analysis.
   *
   * @param label The new label to set for this control flow block.
   */
  void setLabel(string label);

  /**
   * @brief Retrieves the list of instructions within the current block.
   *
   * This function returns a vector containing all the instructions present in this block.
   * Each instruction represents a basic operation in the program's flow, used in program analysis
   * or code generation.
   *
   * @return A vector of `BaseIRInstr *` representing the instructions in the block.
   */
  vector<BaseIRInstr *> getInstr();

  /**
   * @brief Sets the "true" exit point for the current block.
   *
   * This function sets the block that serves as the "true" exit in the control flow of the program.
   * This is often used when there is a conditional branch and the flow of execution
   * diverges depending on the outcome of a condition.
   *
   * @param bb A pointer to the `BasicBlock` that represents the "true" exit of the current block.
   */
  void setExitTrue(BasicBlock *bb);

  /**
   * @brief Sets the "false" exit point for the current block.
   *
   * This function sets the block that serves as the "false" exit in the control flow of the program.
   * This is typically used for conditional branches, where the program flow follows one
   * path if a condition is true, and another path if it is false.
   *
   * @param bb A pointer to the `BasicBlock` that represents the "false" exit of the current block.
   */
  void setExitFalse(BasicBlock *bb);

  /**
   * @brief Retrieves the "true" exit point of the current block.
   *
   * This function returns the basic block representing the "true" exit point in the control flow.
   * It is used to identify where the program flow should continue if a condition evaluates to true.
   *
   * @return A pointer to the `BasicBlock` representing the "true" exit.
   */
  BasicBlock *getExitTrue();

  /**
   * @brief Retrieves the "false" exit point of the current block.
   *
   * This function returns the basic block representing the "false" exit point in the control flow.
   * It is used to identify where the program flow should continue if a condition evaluates to false.
   *
   * @return A pointer to the `BasicBlock` representing the "false" exit.
   */
  BasicBlock *getExitFalse();

  /**
   * @brief Sets the name of the test variable.
   *
   * This function sets the name of the test variable used in the basic block.
   *
   * @param name The name of the test variable.
   */
  void setIsTestVar(bool isTest)
  {
    is_test_var = isTest;
  }

  /**
   * @brief Get the label of the true exit point of the current block.
   *
   * This function returns the label of the "true" exit point in the control flow.
   * It is used to identify where the program flow should continue if a condition evaluates to true.
   *
   * @return A string representing the label of the "true" exit.
   */
  string getTrueLabel();

  /**
   * @brief Get the label of the false exit point of the current block.
   *
   * This function returns the label of the "false" exit point in the control flow.
   * It is used to identify where the program flow should continue if a condition evaluates to false.
   *
   * @return A string representing the label of the "false" exit.
   */
  string getFalseLabel();

protected:
  /// Pointer to the basic block representing the "true" exit.
  /// This is used for the branch or conditional statement when the condition is true.
  /// It can be null if no "true" exit exists.
  BasicBlock *exit_true;

  /// Pointer to the basic block representing the "false" exit.
  /// This is used for the branch or conditional statement when the condition is false.
  /// It can be null if no "false" exit exists.
  BasicBlock *exit_false;

  /// The label for the basic block, also used as the label in the generated assembly code.
  string label;

  /// The control flow graph to which this basic block belongs.
  CFG *cfg;

  /// A vector of instructions that belong to this basic block.
  vector<BaseIRInstr *> instrs;

  /// A flag indicating whether the block is a test variable.
  bool is_test_var = false;
};
