#include <briltype.h>

BrilType::BrilType() {
    this->primitive = BRIL_VOID;
    this->indirection = 0;
}

json BrilType::dump2json() {
    json result = primitive2string(this->primitive);
    for (int i = this->indirection; i > 0; i--) {
        json tmp;
        tmp["ptr"] = result;
        result = tmp;
    }
    return result;
}

std::string type2string(BrilType type) {
    std::string result = "{";
    result += primitive2string(type.primitive);
    result += ", ";
    result += std::to_string(type.indirection);
    result += "}";
    return result;
}

void parseBrilType(BrilType *ptr2type, json data) {
    if (data.is_null() || !data.contains("type")) {
        ptr2type->primitive = BRIL_VOID;
        ptr2type->indirection = 0;
        return;
    }

    int count = 0;
    data = data["type"];
    while (!data.is_string()) {
        data = data["ptr"];
        count++;
    }
    ptr2type->primitive = string2primitive(std::string(data));
    ptr2type->indirection = count;
}
