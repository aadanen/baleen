#ifndef BRIL_OP_H_
#define BRIL_OP_H_

#include <map>
#include <string>

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
};

std::string op2string(BrilOp op);
BrilOp string2op(const std::string &str);
bool isAssociative(BrilOp op);

#endif /* BRIL_OP_H_ */
