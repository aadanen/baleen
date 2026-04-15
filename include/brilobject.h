#ifndef BRIL_OBJECT_H_
#define BRIL_OBJECT_H_

#include <brilop.h>
#include <briltype.h>
#include <json.hpp>
#include <map>
#include <string>

using json = nlohmann::json;

enum BrilFlags {
    BRIL_DEAD = 0x1,
};

struct BrilObject {
    BrilOp op;
    BrilType type;
    int flags;
    union {
        int name; // location in stringtab
        int dest;
    };
    int num_args;
    union {
        int arg0;
        int value;
    };
    union {
        int arg1;
        int func;
    };
    int arg2;
    int num_instrs;

    int width();

    BrilObject();

    int init(json data, BrilOp op);

    json dump2json();
    void print();
    bool islabel();
    bool isterminator();
    bool isfunc();
    bool operator==(const BrilObject &rhs) const;
    bool operator!=(const BrilObject &rhs) const;
    int instr0();
};

struct BrilObjectHasher {
    std::size_t operator()(const BrilObject &k) const;
};

#endif /* BRIL_OBJECT_H_ */
