#ifndef BRIL_TYPE_H_
#define BRIL_TYPE_H_

#include <brilprimitive.h>
#include <json.hpp>
using json = nlohmann::json;

struct BrilType {
    BrilPrimitive primitive;
    int indirection;

    BrilType();
    json dump2json();
};

void parseBrilType(BrilType *ptr2type, json data);
std::string type2string(BrilType type);

#endif /* BRIL_TYPE_H_ */
