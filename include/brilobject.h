#ifndef BRIL_OBJECT_H_
#define BRIL_OBJECT_H_

#include <brilop.h>
#include <briltype.h>
#include <json.hpp>
#include <map>
#include <string>

using json = nlohmann::json;

struct BrilObject {
    BrilOp op;
    BrilType type;
    union {
        int name; // location in stringtab
        int dest;
    };
    int arg0;
    int num_args;
    union {
        int instr0;
        int func0;
    };
    union {
        int num_instrs;
        int num_funcs;
    };
    int label0;
    int num_labels;
    int value;

    int offset();

    BrilObject();

    int init(json data, BrilOp op);

    json dump2json();
    void print();
};

#endif /* BRIL_OBJECT_H_ */
