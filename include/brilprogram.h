#ifndef BRIL_PROGRAM_H_
#define BRIL_PROGRAM_H_

#include <brilbasicblock.h>
#include <brilfunction.h>
#include <brilobject.h>
#include <lvn.h>
#include <set>
#include <stringtable.h>
#include <vector>

struct BrilProgram {
public:
  BrilProgram(json program);
  json dump2json();
  int optimize();
  void data_flow_analysis();
  void find_dominators();

  StringTable stringtable;
  std::vector<BrilObject> objects;
  std::vector<int> var_args;
  std::vector<BrilBasicBlock> blocks;
  std::vector<BrilFunction> functions;
  std::vector<std::vector<int>> dce_cfg;
  std::vector<std::vector<int>> cfg;
  std::vector<std::vector<int>> back_cfg;
  std::vector<std::set<int>> out; // out set of [name, index] pairs
  std::vector<std::set<int>> in;  // in set of [name, index] pairs
  // if df_rename[i] = j, then j is the unique name for the instr at index i
  // in[] and out[] will just hold original names and indicies but then we can
  // look them up the unique names later
  std::vector<int> df_rename;
  // vector of sets per block
  // set of block indices of the dominators of that block in the cfg
  std::vector<std::set<int>> dom;
  std::vector<std::set<int>> strict_dom;
  std::vector<std::set<int>> strict_dom_by;
  std::vector<std::vector<int>> dom_tree;
  std::vector<std::vector<int>> dom_frontier;

private:
  std::map<int, int> blocktable; // map stringtable entry to block index
  void printBlocks();
  void printFunctions();
  void printDominator(std::vector<std::set<int>> &s);
  void printDominator(std::vector<std::vector<int>> &v);
  int getBlocks();
  void getFunctions();
  int getCFG();
  int getDceCFG();
  int markDeadBlocks();
  int markDeadBlocksPass();
  int markDeadFunctions();
  int markUnusedVariables();
  int markUnusedVariablesPass();
  int markUnusedDefinitions();
  int markUnusedDefinitionsPass();
  int eliminateDeadCode();
  int local_value_numbering();
  void df_merge(int i);
  bool df_transfer(int i);
};

extern BrilProgram *curr_program;

#endif /* BRIL_PROGRAM_H_ */
