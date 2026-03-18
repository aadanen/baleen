#include <algorithm>
#include <brilobject.h>
#include <brilprogram.h>
#include <iostream>

BrilProgram *curr_program = nullptr;

BrilProgram::BrilProgram(json program) {
  curr_program = this;
  this->objects = std::vector<BrilObject>();
  this->objects.emplace_back(); // the objects array is one indexed
  this->stringtable = StringTable();
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

