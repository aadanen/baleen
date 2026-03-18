#ifndef BRIL_OBJECT_H_
#define BRIL_OBJECT_H_

#include <json.hpp>
#include <map>
#include <string>

using json = nlohmann::json;

enum BrilType {
    BRIL_VOID = 0,
    BRIL_INT,
    BRIL_BOOL,
};
extern std::map<std::string, BrilType> typetable;

enum BrilOp {
    BRIL_NONE = 0,
    BRIL_CONST,
    BRIL_NOP,
    BRIL_ADD,
    BRIL_SUB,
    BRIL_MUL,
    BRIL_DIV,
    BRIL_EQ,
    BRIL_LT,
    BRIL_GT,
    BRIL_LE,
    BRIL_GE,
    BRIL_NOT,
    BRIL_AND,
    BRIL_OR,
    BRIL_JMP,
    BRIL_BR,
    BRIL_CALL,
    BRIL_RET,
    BRIL_ID,
    BRIL_PRINT,
    BRIL_ALLOC,
    BRIL_FREE,
    BRIL_LOAD,
    BRIL_STORE,
    BRIL_PTRADD,
    // not real opcodes
    BRIL_LABEL,
    BRIL_FUNC,
    BRIL_ARG,
    BRIL_STR,
};
extern std::map<std::string, BrilOp> optable;

struct BrilObjectType {
    BrilType primitive;
    int indirection;

    BrilObjectType();
    json dump2json();
};

BrilType string2type(std::string str);
void parseBrilObjectType(BrilObjectType *ptr2type, json data);

struct BrilObject {
    BrilOp op;
    BrilObjectType type;
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
