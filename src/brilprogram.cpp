#include <brilbasicblock.h>
#include <brilprogram.h>
#include <iostream>

BrilProgram *curr_program = nullptr;

BrilProgram::BrilProgram(json program) {
  curr_program = this;
  objects = std::vector<BrilObject>();
  objects.emplace_back(); // the objects array is one indexed
  stringtable = StringTable();
  for (json func : program["functions"]) {
    int i = curr_program->objects.size();
    curr_program->objects.push_back(BrilObject());
    curr_program->objects[i].init(func, BRIL_FUNC);
  }

  curr_program = nullptr;
}

json BrilProgram::dump2json() {
  curr_program = this;
  json result;
  std::vector<json> functions;
  for (int i = 1; i < objects.size(); i++) {
    if (objects[i].op == BRIL_FUNC) {
      json j = objects[i].dump2json();
      functions.push_back(j);
    }
  }
  result["functions"] = functions;
  curr_program = nullptr;
  return result;
};

int BrilProgram::getBlocks() {
  curr_program = this;
  int bname = 0;
  int bstart = 0;
  int blength = 0;
  for (int index = 1; index < objects.size(); index++) {
    if (this->objects[index].op == BRIL_ARG)
      continue;

    if (this->objects[index].isfunc() || this->objects[index].islabel()) {
      if (bstart > 0) {
        this->blocks.emplace_back(bname, bstart, blength);
      }

      bname = this->objects[index].name;
      this->blocktable[bname] = this->blocks.size();
      if (this->objects[index].isfunc())
        bstart = this->objects[index].arg0 + this->objects[index].num_args;
      else
        bstart = index + 1;
      blength = 0;
    } else {
      blength++;
      if (this->objects[index].isterminator()) {
        this->blocks.emplace_back(bname, bstart, blength);
        bname = 0;
        bstart = 0;
        blength = 0;
      }
    }
  }
  if (bstart > 0)
    this->blocks.emplace_back(bname, bstart, blength);

  for (BrilBasicBlock &b : this->blocks) {
    b.print();
  }
  curr_program = nullptr;
  return 0;
}

int BrilProgram::getCFG() {
  if (blocks.empty())
    getBlocks();

  cfg = std::vector<std::vector<int>>(blocks.size(), std::vector<int>());
  for (int i = 0; i < blocks.size(); i++) {
    // get the last instruction;
    int last_instr = blocks[i].start + blocks[i].length - 1;
    if (objects[last_instr].isterminator()) {
      if (objects[last_instr].op == BRIL_JMP) {
        cfg[i].push_back(blocktable[objects[last_instr].arg0]);
      } else if (objects[last_instr].op == BRIL_BR) {
        cfg[i].push_back(blocktable[objects[last_instr].arg1]);
        cfg[i].push_back(blocktable[objects[last_instr].arg2]);
      } // else its a ret instruction and so control goes to some caller
    } else {
      if (last_instr + 1 < objects.size() &&
          objects[last_instr + 1].op == BRIL_LABEL) {
        cfg[i].push_back(blocktable[objects[last_instr + 1].name]);
      }
    }
  }

  int counter = 0;
  for (std::vector<int> &row : cfg) {
    std::cout << counter << ": [";
    for (int &item : row) {
      std::cout << " " << item << " ";
    }
    std::cout << "]\n";
    counter++;
  }
  return 0;
}

int BrilProgram::numDeadBlocks() {
  if (cfg.empty())
    getCFG();

  // this bit assumes dead until proven alive so imma just mark them with
  // BRIL_DEAD but then flip them all over so the logic works

  // other blocks are alive if they can be transitioned into
  std::vector<bool> alive(blocks.size(), false);
  for (int i = 0; i < cfg.size(); i++) {
    // dead blocks can't make subsequent blocks alive
    if (!(blocks[i].flags & BRIL_DEAD)) {
      for (int j = 0; j < cfg[i].size(); j++) {
        alive[cfg[i][j]] = true;
      }
    }
  }

  int counter = 0;
  for (int i = 0; i < blocks.size(); i++) {
    // main is always alive
    if (stringtable.getString(blocks[i].name) == "main") {
      alive[i] = true;
    }
    if (!alive[i]) {
      blocks[i].flags |= BRIL_DEAD;
      counter++;
    }
  }
  return counter;
}

// maybe i want to markDeadCode and then separately eliminate dead code?
// i feel like i need to see more optimizations before i can reason about a
// general interface for compiler optimizations
int BrilProgram::eliminateDeadCode() {
  int prev_dead_code = -1;
  int total_dead_code = 0;
  int tmp = 0;
  while (prev_dead_code != total_dead_code) {
    prev_dead_code = total_dead_code;
    while ((tmp = numDeadBlocks()) > 0) {
      total_dead_code += tmp;
    }
    // if block[i] is dead, mark all the instructions in block[i] as dead
    for (BrilBasicBlock b : blocks) {
      if (b.flags & BRIL_DEAD) {
        for (int i = b.start; i < b.start + b.length; i++) {
          objects[i].flags |= BRIL_DEAD;
        }
      }
    }
  }


  // make a new objects array without the dead instructions
  // update functions with their new number of instructions
  std::vector<BrilObject> new_objects;
  new_objects.emplace_back(); // 1 indexing
  int old_func_idx = 1;
  int new_func_idx = 1;
  while (old_func_idx < objects.size()) {
    // copy function object
    new_objects.push_back(objects[old_func_idx]);
    
    // copy function arguments
    for (int i = old_func_idx + 1; i < objects[old_func_idx].num_args; i++) {
      new_objects.push_back(objects[i]);
    }

    // copy alive instructions and decrement num_instrs by num dead instrs
    for (int i = (objects[old_func_idx].arg0 + objects[old_func_idx].num_args);
        i < objects[old_func_idx].num_instrs;
        i++) {
      if (objects[i].flags & BRIL_DEAD) {
        new_objects[old_func_idx].num_instrs--;
      } else {
        new_objects.push_back(objects[i]); 
      }
    }

    old_func_idx += objects[old_func_idx].num_args + objects[old_func_idx].num_instrs + 1;
    new_func_idx += new_objects[new_func_idx].num_args + new_objects[new_func_idx].num_instrs + 1;
  }
  objects = new_objects;
  blocks.clear();
  cfg.clear();
  return 0;
}
