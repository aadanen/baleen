#include <baleen_utils.h>
#include <brilbasicblock.h>
#include <brilprogram.h>
#include <iostream>
#include <set>
#include <unordered_map>

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

void BrilProgram::printBlocks() {
  for (BrilBasicBlock &b : this->blocks) {
    b.print();
  }
}

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
      if (bstart == 0) {
        bstart = index;
      }
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

  printBlocks();
  curr_program = nullptr;
  return 0;
}

int BrilProgram::getCFG() {
  if (blocks.empty())
    getBlocks();

  cfg = std::vector<std::vector<int>>(blocks.size(), std::vector<int>());
  for (int i = 0; i < blocks.size(); i++) {
    // for blocks we depend on via function calls
    for (int j = blocks[i].start; j < blocks[i].start + blocks[i].length; j++) {
      if (objects[j].op == BRIL_CALL && !(objects[j].flags & BRIL_DEAD)) {
        cfg[i].push_back(blocktable[objects[j].func]);
      }
    }
    // for branch at the end of the block
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

  /*
  int counter = 0;
  for (std::vector<int> &row : cfg) {
    std::cout << counter << ": [";
    for (int &item : row) {
      std::cout << " " << item << " ";
    }
    std::cout << "]\n";
    counter++;
  }
  */
  return 0;
}

int BrilProgram::markDeadBlocksPass() {

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
    if (!alive[i] && !(blocks[i].flags & BRIL_DEAD)) {
      blocks[i].flags |= BRIL_DEAD;
      counter++;
    }
  }
  return counter;
}

int BrilProgram::markDeadBlocks() {
  if (cfg.empty())
    getCFG();
  int prev_dead_code = -1;
  int total_dead_code = 0;
  int tmp = 0;
  while (prev_dead_code != total_dead_code) {
    prev_dead_code = total_dead_code;
    while ((tmp = markDeadBlocksPass()) > 0) {
      total_dead_code += tmp;
    }
    if (total_dead_code == prev_dead_code)
      break;
    // if block[i] is dead, mark all the instructions in block[i] as dead
    for (BrilBasicBlock b : blocks) {
      if (b.flags & BRIL_DEAD) {
        int i = b.start;
        if (b.start > 1 && (objects[b.start - 1].op == BRIL_ARG)) {
          while (objects[i - 1].op != BRIL_FUNC) {
            i--;
          }
        }
        if (i > 1 &&
            (objects[i - 1].op == BRIL_LABEL || objects[i - 1].op == BRIL_FUNC))
          i--;
        for (i; i < b.start + b.length; i++) {
          objects[i].flags |= BRIL_DEAD;
        }
      }
    }
  }
  return total_dead_code;
}

// maybe i want to markDeadCode and then separately eliminate dead code?
// i feel like i need to see more optimizations before i can reason about a
// general interface for compiler optimizations
int BrilProgram::eliminateDeadCode() {
  curr_program = this;
  // make a new objects array without the dead instructions
  // update functions with their new number of instructions
  std::vector<BrilObject> new_objects;
  new_objects.emplace_back(); // 1 indexing
  int old_func_idx = 1;
  int new_func_idx = 1;
  int offset = 0;
  while (old_func_idx < objects.size()) {
    // copy function object
    if (!(objects[old_func_idx].flags & BRIL_DEAD)) {
      new_objects.push_back(objects[old_func_idx]);
      new_objects[new_func_idx].arg0 -= offset;

      // copy function arguments
      for (int i = old_func_idx + 1;
           i < old_func_idx + 1 + objects[old_func_idx].num_args; i++) {
        new_objects.push_back(objects[i]);
      }

      // copy alive instructions and decrement num_instrs by num dead instrs
      int start = objects[old_func_idx].arg0 + objects[old_func_idx].num_args;
      for (int i = start; i < start + objects[old_func_idx].num_instrs; i++) {
        if (objects[i].flags & BRIL_DEAD) {
          offset++;
          new_objects[new_func_idx].num_instrs--;
        } else {
          new_objects.push_back(objects[i]);
        }
      }
      old_func_idx += objects[old_func_idx].width();
      new_func_idx += new_objects[new_func_idx].width();
    } else {
      int w = objects[old_func_idx].width();
      offset += w;
      old_func_idx += w;
    }
  }
  objects = new_objects;
  blocks.clear();
  cfg.clear();
  curr_program = nullptr;
  return 0;
}

int BrilProgram::markUnusedVariablesPass() {
  // each function handled independtly to respect scope rules
  int counter = 0;
  for (int func_idx = 1; func_idx < objects.size();
       func_idx += objects[func_idx].width()) {
    int instr0 = objects[func_idx].instr0();
    // set of stringtable indicies of used variables
    std::set<int> used;
    // read all instr args to see what vars are used
    for (int _i = 0; _i < objects[func_idx].num_instrs; _i++) {
      int i = instr0 + _i;
      if (objects[i].op == BRIL_LABEL || (objects[i].flags & BRIL_DEAD))
        continue;

      int *p;
      if (objects[i].op == BRIL_CALL)
        p = &var_args[objects[i].arg0];
      else
        p = &objects[i].arg0;

      for (int j = 0; j < objects[i].num_args; j++) {
        used.insert(p[j]);
      }
    }

    // examine all variables (dest) and mark dead if not used
    for (int _i = 0; _i < objects[func_idx].num_instrs; _i++) {
      int i = instr0 + _i;
      if (objects[i].op == BRIL_LABEL)
        continue;

      if (objects[i].dest && (used.find(objects[i].dest) == used.end()) &&
          !(objects[i].flags & BRIL_DEAD)) {
        objects[i].flags |= BRIL_DEAD;
        counter++;
      }
    }
  }
  return counter;
}

int BrilProgram::markUnusedVariables() {
  int total = 0;
  int tmp;
  while ((tmp = markUnusedVariablesPass()) > 0)
    total += tmp;
  return total;
}

int BrilProgram::markUnusedDefinitionsPass() {
  int counter = 0;
  for (BrilBasicBlock &b : blocks) {
    // map from var to objects index
    std::map<int, int> last_def;

    // for each variable keep track of the most recent definition
    // if I redfine a variable then mark the old one as used
    // whenever I use an instruction as an argument remove its definition
    for (int i = b.start; i < b.start + b.length; i++) {
      if (objects[i].op == BRIL_LABEL || (objects[i].flags & BRIL_DEAD))
        continue;

      int *p;
      if (objects[i].op == BRIL_CALL)
        p = &var_args[objects[i].arg0];
      else
        p = &objects[i].arg0;

      // remove uses
      for (int j = 0; j < objects[i].num_args; j++) {
        last_def.erase(p[j]);
      }

      // register definitions
      if (last_def.find(objects[i].dest) != last_def.end()) {
        // if there was a prev definition, mark as dead
        objects[last_def[objects[i].dest]].flags |= BRIL_DEAD;
        counter++;
      }
      if (objects[i].dest)
        last_def[objects[i].dest] = i;
    }
  }
  return counter;
}

int BrilProgram::markUnusedDefinitions() {
  if (blocks.empty())
    getBlocks();
  int total = 0;
  int tmp;
  while ((tmp = markUnusedDefinitionsPass()) > 0)
    total += tmp;
  return total;
}

// also tries to do copy and constant propogation
BrilObject lvn_copyof(int name, const BrilObject *obj) {
  BrilObject result = BrilObject();
  result.dest = name;
  result.type = obj->type;
  result.flags = obj->flags;
  if (obj->op == BRIL_CONST) {
    result.op = BRIL_CONST;
    result.value = obj->value;
    result.num_args = 0;
  } else if (obj->op == BRIL_ID) {
    result.op = BRIL_ID;
    result.arg0 = obj->arg0;
    result.num_args = 1;
  } else {
    result.op = BRIL_ID;
    result.arg0 = obj->dest;
    result.num_args = 1;
  }
  return result;
}

void lvn_renameArgs(BrilObject &obj, std::map<int, int> &rename) {
  int *p = &obj.arg0;
  for (int j = 0; j < obj.num_args; j++) {
    if (rename.find(p[j]) != rename.end()) {
      p[j] = rename[p[j]];
    }
  }
}

int BrilProgram::local_value_numbering() {
  if (blocks.empty())
    getBlocks();

  for (BrilBasicBlock &b : blocks) {
    LVN lvn = LVN();
    int fdx = b.start;
    while (fdx >= 0) { // register the current function arguments
      if (objects[fdx].op == BRIL_FUNC)
        break;

      if (objects[fdx].op == BRIL_ARG) {
        BrilObject obj = BrilObject();
        obj.op = BRIL_ID;
        obj.num_args = 1;
        obj.name = objects[fdx].name;
        obj.arg0 = objects[fdx].name; // negative cuz its not a vn
        int vn = lvn.insert(obj);
        lvn.updateEnv(objects[fdx].name, vn);
      }
      fdx--;
    }
    // precompute the last definition of each variable
    std::map<int, int> last_def;
    std::map<int, int> rename;
    for (int i = b.start; i < b.start + b.length; i++) {
      if (objects[i].dest) {
        last_def[objects[i].dest] = i;
      }
    }

    for (int i = b.start; i < b.start + b.length; i++) {
      if (objects[i].dest) {
        // erase renamings on redefinition
        if (rename.find(objects[i].dest) != rename.end())
          rename.erase(objects[i].dest);

        // setup renaming for all new definitions
        if (i < last_def[objects[i].dest]) {
          int original = objects[i].dest;
          objects[i].dest = uniqueRename(*this, objects[i].dest);
          rename[original] = objects[i].dest;
        }
      }

      if (isAssociative(objects[i].op)) {
        if (objects[i].arg0 > objects[i].arg1) {
          std::swap(objects[i].arg0, objects[i].arg1);
        }
      }

      lvn_renameArgs(objects[i], rename);

      // can't optimize calls or put them in the lvn table
      if (objects[i].op == BRIL_CALL) {
        continue;
      }

      // lvn table instr has value numbers instead of variable names
      BrilObject obj = objects[i];
      int *p = &obj.arg0;
      int original_arg0 = p[0];
      for (int j = 0; j < obj.num_args; j++) {
        int arg_vn = lvn.lookupVN(p[j]);
        if (arg_vn >= 0) {
          p[j] = arg_vn;
        } else {
          if (rename.find(p[j]) != rename.end()) {
            p[j] = rename[p[j]];
          }
          p[j] = -p[j]; // if not found, just -name for reconstruction later
        }
      }

      int vn = lvn.lookupVN(obj);
      if (vn >= 0) {
        if (objects[i].dest)
          objects[i] = lvn_copyof(objects[i].dest, lvn.lookupInstr(vn));
      } else {
        if (objects[i].op == BRIL_ID) { // copies aren't generating values
          vn = lvn.lookupVN(original_arg0);
          objects[i] = lvn_copyof(objects[i].dest, lvn.lookupInstr(vn));
        } else {
          // insert a new row into the lvn table
          vn = lvn.insert(obj);

          // modify the current instruction to use the arguments from the vn
          int *p = &objects[i].arg0;
          int *q = &obj.arg0;
          const BrilObject *arg_instr;
          for (int j = 0; j < objects[i].num_args; j++) {
            if (q[j] >= 0) {
              arg_instr = lvn.lookupInstr(q[j]);
              if (arg_instr && arg_instr->dest > 0) {
                p[j] = arg_instr->dest; // lvn -> modified instr -> arg_vn ->
                                        // arg_instr -> true name
              } else {
                std::cout << "error: lvn shouldn't be here i think LOL\n";
                return -1;
              }
            } else { // its some unknown value that we -named earlier
              p[j] = -q[j];
            }
          }
        }
      }
      if (objects[i].dest)
        lvn.updateEnv(objects[i].dest, vn);
    }
  }
  return 0;
}

int BrilProgram::optimize() {
  local_value_numbering();
  int tmp = 1;
  while (tmp) {
    tmp = 0;
    std::cout << "START\n";
    tmp += markUnusedVariables();
    curr_program = this;
    printBlocks();
    tmp += markUnusedDefinitions();
    curr_program = this;
    printBlocks();
    tmp += markDeadBlocks();
    curr_program = this;
    printBlocks();
  }
  eliminateDeadCode();
  curr_program = nullptr;
  return 0;
}
