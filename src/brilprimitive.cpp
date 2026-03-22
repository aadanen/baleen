#include <brilprimitive.h>
#include <iostream>
#include <map>

std::map<std::string, BrilPrimitive> primtable = {
    {"void", BRIL_VOID},
    {"int", BRIL_INT},
    {"bool", BRIL_BOOL},
};

std::map<BrilPrimitive, std::string> primstringtable = {
    {BRIL_VOID, "void"},
    {BRIL_INT, "int"},
    {BRIL_BOOL, "bool"},
};

BrilPrimitive string2primitive(std::string str) {
    if (primtable.find(str) == primtable.end()) {
        std::cerr << "Error: illegal string2primitive lookup\n";
        exit(1);
    }
    return primtable[str];
}
std::string primitive2string(BrilPrimitive type) {
    if (primstringtable.find(type) == primstringtable.end()) {
        std::cerr << "Error: illegal primitive2string lookup\n";
        exit(1);
    }
    return primstringtable[type];
}
