#ifndef BRIL_PROGRAM_H_
#define BRIL_PROGRAM_H_

#include <brilbasicblock.h>
#include <brilobject.h>
#include <stringtable.h>
#include <vector>

struct BrilProgram {
public:
  BrilProgram(json program);
  json dump2json();
  int optimize();

  StringTable stringtable;
  std::vector<BrilObject> objects;
  std::vector<int> var_args;
  std::vector<BrilBasicBlock> blocks;
  std::vector<std::vector<int>> cfg;

private:
  std::map<int, int> blocktable; // map stringtable entry to block index
  int getBlocks();
  int getCFG();
  int markDeadBlocks();
  int markDeadBlocksPass();
  int markDeadFunctions();
  int markUnusedVariables();
  int markUnusedVariablesPass();
  int eliminateDeadCode();
};

extern BrilProgram *curr_program;

#endif /* BRIL_PROGRAM_H_ */
