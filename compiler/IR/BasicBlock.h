#pragma once

#include "CFG.h"
#include "Instr/BaseIRInstr.h"
#include <vector>

using namespace std;

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
  void gen_asm(ostream &o);

  /**
   * @brief Adds an instruction to the basic block.
   *
   * This method adds a new intermediate representation instruction (IRInstr) to the basic block.
   *
   * @param instr A pointer to the instruction to add.
   */
  void add_IRInstr(BaseIRInstr *instr);

  /**
   * @brief Gets the CFG associated with this basic block.
   *
   * This method retrieves the CFG to which this basic block belongs.
   *
   * @return A pointer to the CFG associated with this basic block.
   */
  CFG *getCFG();

protected:
  /// The label for the basic block, also used as the label in the generated assembly code.
  string label;

  /// The control flow graph to which this basic block belongs.
  CFG *cfg;

  /// A vector of instructions that belong to this basic block.
  vector<BaseIRInstr *> instrs;
};
