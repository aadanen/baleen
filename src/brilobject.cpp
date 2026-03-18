#include <brilobject.h>
#include <brilprogram.h>
#include <iostream>

std::map<std::string, BrilType> typetable = {
    {"int", BRIL_INT},
    {"bool", BRIL_BOOL},
};

std::map<BrilType, std::string> typestringtable = {
    {BRIL_INT, "int"},
    {BRIL_BOOL, "bool"},
};

std::map<std::string, BrilOp> optable = {
    {"nop", BRIL_NOP},       {"add", BRIL_ADD},     {"sub", BRIL_SUB},
    {"mul", BRIL_MUL},       {"div", BRIL_DIV},     {"eq", BRIL_EQ},
    {"lt", BRIL_LT},         {"gt", BRIL_GT},       {"le", BRIL_LE},
    {"ge", BRIL_GE},         {"not", BRIL_NOT},     {"and", BRIL_AND},
    {"or", BRIL_OR},         {"jmp", BRIL_JMP},     {"br", BRIL_BR},
    {"call", BRIL_CALL},     {"ret", BRIL_RET},     {"id", BRIL_ID},
    {"print", BRIL_PRINT},   {"const", BRIL_CONST}, {"alloc", BRIL_ALLOC},
    {"free", BRIL_FREE},     {"load", BRIL_LOAD},   {"store", BRIL_STORE},
    {"ptradd", BRIL_PTRADD},
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
    {BRIL_PTRADD, "ptradd"},
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

void addObject(json data, BrilOp op) {
    int i = curr_program->objects.size();
    curr_program->objects.push_back(BrilObject());
    curr_program->objects[i].init(data, op);
}
// TODO: replace table lookups with getters that check args
// TODO: set instr0, arg0, num_instr, etc...
BrilObjectType::BrilObjectType() {
    this->primitive = BRIL_VOID;
    this->indirection = 0;
}

BrilType string2type(std::string str) {
    if (typetable.find(str) == typetable.end()) {
        std::cerr << "Error: illegal string2type lookup\n";
        exit(1);
    }
    return typetable[str];
}
std::string type2string(BrilType type) {
    if (typestringtable.find(type) == typestringtable.end()) {
        std::cerr << "Error: illegal type2string lookup\n";
        exit(1);
    }
    return typestringtable[type];
}
void parseBrilObjectType(BrilObjectType *ptr2type, json data) {
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
    ptr2type->primitive = string2type(std::string(data));
    ptr2type->indirection = count;
}

json BrilObjectType::dump2json() {
    json result = type2string(this->primitive);
    for (int i = this->indirection; i > 0; i--) {
        json tmp;
        tmp["ptr"] = result;
        result = tmp;
    }
    return result;
}

BrilObject::BrilObject() {
    this->op = BRIL_NONE;
    this->type = BrilObjectType();
    this->name = 0;
    this->arg0 = 0;
    this->num_args = 0;
    this->instr0 = 0;
    this->num_instrs = 0;
    this->label0 = 0;
    this->num_labels = 0;
    this->value = 0;
}

int BrilObject::init(json data, BrilOp op) {
    this->op = BRIL_NONE;
    this->type = BrilObjectType();
    this->name = 0;
    this->arg0 = 0;
    this->num_args = 0;
    this->instr0 = 0;
    this->num_instrs = 0;
    this->label0 = 0;
    this->num_labels = 0;
    this->value = 0;
    int self_index = curr_program->objects.size() - 1;
    BrilObject *self = &(curr_program->objects[self_index]);
    switch (op) {
    case (BRIL_FUNC):
        /*
        {
         "name": "<string>",
         "type": <Type>?,
         "args": [{"name": "<string>", "type": <Type>}, ...]?,
         "instrs": [<Instruction>, ...]
        }
        */
        self->op = BRIL_FUNC;
        self->name = curr_program->stringtable.lookup(data["name"]);

        parseBrilObjectType(&self->type, data);

        if (data.contains("args") && !data["args"].empty()) {
            self->arg0 = curr_program->objects.size();
            for (json arg : data["args"]) {
                addObject(arg, BRIL_ARG);
                self = &curr_program->objects[self_index];
                self->num_args++;
            }
        }
        self->instr0 = curr_program->objects.size();
        for (json instr : data["instrs"]) {
            if (instr.contains("label")) {
                addObject(instr, BRIL_LABEL);
                self = &curr_program->objects[self_index];
            } else {
                addObject(instr, string2op(instr["op"]));
                self = &curr_program->objects[self_index];
            }
            self->num_instrs++;
        }
        break;
    case (BRIL_ARG):
        /*
         {"name": "<string>", "type": <Type>}
        */
        self->op = BRIL_ARG;
        parseBrilObjectType(&self->type, data);
        self->name = curr_program->stringtable.lookup(data["name"]);
        break;
    case (BRIL_LABEL):
        self->op = BRIL_LABEL;
        self->name = curr_program->stringtable.lookup(data["label"]);
        break;
    case (BRIL_STR):
        /*
         "string"
        */
        self->op = BRIL_STR;
        parseBrilObjectType(&self->type, data);
        self->name = curr_program->stringtable.lookup(data);
        break;
    default:
        self->op = string2op(data["op"]);
        if (data.contains("dest"))
            self->dest = curr_program->stringtable.lookup(data["dest"]);
        if (data.contains("type"))
            parseBrilObjectType(&self->type, data);
        if (data.contains("args") && !data["args"].empty()) {
            self->arg0 = curr_program->objects.size();
            for (json arg : data["args"]) {
                addObject(arg, BRIL_STR);
                self = &curr_program->objects[self_index];
                self->num_args++;
            }
        }
        if (data.contains("funcs") && !data["funcs"].empty()) {
            self->func0 = curr_program->objects.size();
            for (json func : data["funcs"]) {
                addObject(func, BRIL_STR);
                self = &curr_program->objects[self_index];
                self->num_funcs++;
            }
        }
        if (data.contains("labels") && !data["labels"].empty()) {
            self->label0 = curr_program->objects.size();
            for (json label : data["labels"]) {
                addObject(label, BRIL_STR);
                self = &curr_program->objects[self_index];
                self->num_labels++;
            }
        }
        if (data.contains("value")) {
            self->value = data["value"];
        }
        break;
    };
    return 0;
}

std::vector<json> serializeArray(int start, int n) {
    std::vector<json> result;
    result.reserve(n);
    for (int i = 0; i < n; i++) {
        result.push_back(curr_program->objects[start + i].dump2json());
    }
    return result;
}

json BrilObject::dump2json() {
    json result;
    std::vector<json> instrs_vec;
    int index;
    switch (op) {
    case (BRIL_FUNC):
        /*
        {
         "name": "<string>",
         "type": <Type>?,
         "args": [{"name": "<string>", "type": <Type>}, ...]?,
         "instrs": [<Instruction>, ...]
        }
        */
        result["name"] = curr_program->stringtable.getString(this->name);

        if (this->type.primitive) {
            result["type"] = this->type.dump2json();
        }

        result["args"] = serializeArray(this->arg0, this->num_args);

        instrs_vec.reserve(this->num_instrs);
        index = 0;
        for (int i = 0; i < this->num_instrs; i++) {
            instrs_vec.push_back(
                curr_program->objects[instr0 + index].dump2json());
            index += curr_program->objects[instr0 + index].offset();
        }
        result["instrs"] = instrs_vec;
        return result;
        break;
    case (BRIL_ARG):
        /*
         {"name": "<string>", "type": <Type>}
        */
        result["name"] = curr_program->stringtable.getString(this->name);
        result["type"] = this->type.dump2json();
        return result;
        break;
    case (BRIL_LABEL):
        // { "label": "<string>" }
        result["label"] = curr_program->stringtable.getString(this->name);
        return result;
        break;
    case (BRIL_STR):
        /*
         "string"
        */
        result = curr_program->stringtable.getString(this->name);
        return result;
        break;
    default:
        result["op"] = op2string(this->op);
        if (this->type.primitive != BRIL_VOID) {
            result["type"] = this->type.dump2json();
        }
        if (this->dest)
            result["dest"] = curr_program->stringtable.getString(this->dest);

        // TODO here unmarshall instruction arguments
        if (this->arg0) {
            result["args"] = serializeArray(this->arg0, this->num_args);
        }

        if (this->func0) {
            result["funcs"] = serializeArray(this->func0, this->num_funcs);
        }

        if (this->label0) {
            result["labels"] = serializeArray(this->label0, this->num_labels);
        }

        if (this->op == BRIL_CONST) {
            result["value"] = this->value;
        }
        return result;
        break;
    };
}

// what if the args have args
int BrilObject::offset() {
    if (this->op == BRIL_FUNC) {
        return num_args + num_instrs + 1;
    } else {
        return num_args + num_funcs + num_labels + 1;
    }
}

void BrilObject::print() {
    std::cout << "op: " << op2string(this->op) << '\n';
    std::cout << "type: " << "{" << this->type.primitive << ", "
              << this->type.indirection << "}" << '\n';
    std::cout << "name: " << curr_program->stringtable.getString(this->name)
              << '\n';
    std::cout << "arg0: " << this->arg0 << '\n';
    std::cout << "num_args: " << this->num_args << '\n';
    std::cout << "instr0/func0: " << this->instr0 << '\n';
    std::cout << "num_instrs/num_funcs: " << this->num_instrs << '\n';
    std::cout << "label0: " << this->label0 << '\n';
    std::cout << "num_labels: " << this->num_labels << '\n';
    std::cout << "value: " << this->value << '\n';
    std::cout << '\n';
}
