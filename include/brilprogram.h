#ifndef BRIL_PROGRAM_H_
#define BRIL_PROGRAM_H_

#include <brilobject.h>
#include <json.hpp>
#include <stringtable.h>
#include <vector>

struct BrilProgram {
    StringTable stringtable;
    std::vector<BrilObject> objects;
    BrilProgram(json program);
    json dump2json();
};

extern BrilProgram *curr_program;

#endif /* BRIL_PROGRAM_H_ */
