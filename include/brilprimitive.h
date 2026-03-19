#ifndef BRIL_PRIMITIVE_H_
#define BRIL_PRIMITIVE_H_

#include <string>

enum BrilPrimitive {
    BRIL_VOID = 0,
    BRIL_INT,
    BRIL_BOOL,
};

BrilPrimitive string2primitive(std::string str);
std::string primitive2string(BrilPrimitive type);

#endif /* BRIL_PRIMITIVE_H_ */
