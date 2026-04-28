#include <brilop.h>
#include <iostream>

std::map<std::string, BrilOp> optable = {
    {"nop", BRIL_NOP},       {"add", BRIL_ADD},     {"sub", BRIL_SUB},
    {"mul", BRIL_MUL},       {"div", BRIL_DIV},     {"eq", BRIL_EQ},
    {"lt", BRIL_LT},         {"gt", BRIL_GT},       {"le", BRIL_LE},
    {"ge", BRIL_GE},         {"not", BRIL_NOT},     {"and", BRIL_AND},
    {"or", BRIL_OR},         {"jmp", BRIL_JMP},     {"br", BRIL_BR},
    {"call", BRIL_CALL},     {"ret", BRIL_RET},     {"id", BRIL_ID},
    {"print", BRIL_PRINT},   {"const", BRIL_CONST}, {"alloc", BRIL_ALLOC},
    {"free", BRIL_FREE},     {"load", BRIL_LOAD},   {"store", BRIL_STORE},
    {"ptradd", BRIL_PTRADD}, {"func", BRIL_FUNC},   {"arg", BRIL_ARG},
    {"label", BRIL_LABEL},   {"set", BRIL_SET},     {"get", BRIL_GET},
    {"undef", BRIL_UNDEF},
};

std::map<BrilOp, std::string> opstringtable = {
    {BRIL_NOP, "nop"},       {BRIL_ADD, "add"},     {BRIL_SUB, "sub"},
    {BRIL_MUL, "mul"},       {BRIL_DIV, "div"},     {BRIL_EQ, "eq"},
    {BRIL_LT, "lt"},         {BRIL_GT, "gt"},       {BRIL_LE, "le"},
    {BRIL_GE, "ge"},         {BRIL_NOT, "not"},     {BRIL_AND, "and"},
    {BRIL_OR, "or"},         {BRIL_JMP, "jmp"},     {BRIL_BR, "br"},
    {BRIL_CALL, "call"},     {BRIL_RET, "ret"},     {BRIL_ID, "id"},
    {BRIL_PRINT, "print"},   {BRIL_CONST, "const"}, {BRIL_ALLOC, "alloc"},
    {BRIL_FREE, "free"},     {BRIL_LOAD, "load"},   {BRIL_STORE, "store"},
    {BRIL_PTRADD, "ptradd"}, {BRIL_FUNC, "func"},   {BRIL_ARG, "arg"},
    {BRIL_LABEL, "label"},   {BRIL_SET, "set"},     {BRIL_GET, "get"},
    {BRIL_UNDEF, "undef"},
};

std::string op2string(BrilOp op) {
    if (opstringtable.find(op) != opstringtable.end()) {
        return opstringtable[op];
    } else {
        std::cerr << "Error: illegal op2string lookup\n";
        exit(1);
    }
}

BrilOp string2op(const std::string &str) {
    if (optable.find(str) != optable.end()) {
        return optable[str];
    } else {
        std::cerr << "Error: illegal string2op lookup\n";
        exit(1);
    }
}

bool isAssociative(BrilOp op) {
    switch (op) {
    case BRIL_ADD:
    case BRIL_MUL:
    case BRIL_EQ:
        return true;
    default:
        return false;
    };
}
