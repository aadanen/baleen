#include <brilobject.h>
#include <brilprogram.h>
#include <iostream>

void addObject(json data, BrilOp op) {
    int i = curr_program->objects.size();
    curr_program->objects.push_back(BrilObject());
    curr_program->objects[i].init(data, op);
}

BrilObject::BrilObject() {
    this->op = BRIL_NONE;
    this->type = BrilType();
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
    this->type = BrilType();
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

        parseBrilType(&self->type, data);

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
        parseBrilType(&self->type, data);
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
        parseBrilType(&self->type, data);
        self->name = curr_program->stringtable.lookup(data);
        break;
    default:
        self->op = string2op(data["op"]);
        if (data.contains("dest"))
            self->dest = curr_program->stringtable.lookup(data["dest"]);
        if (data.contains("type"))
            parseBrilType(&self->type, data);
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
