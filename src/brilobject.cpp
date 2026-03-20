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
    this->num_args = 0;
    this->arg0 = 0;
    this->arg1 = 0;
    this->arg2 = 0;
    this->num_instrs = 0;
    this->value = 0;
}

int BrilObject::init(json data, BrilOp op) {
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

        self->arg0 = curr_program->objects.size();
        if (data.contains("args") && !data["args"].empty()) {
            for (json arg : data["args"]) {
                addObject(arg, BRIL_ARG);
                self = &curr_program->objects[self_index];
                self->num_args++;
            }
        }
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
        self->num_args = 1;
        break;
    default:
        self->op = string2op(data["op"]);
        if (data.contains("dest"))
            self->dest = curr_program->stringtable.lookup(data["dest"]);
        if (data.contains("type"))
            parseBrilType(&self->type, data);
        if (data.contains("value")) {
            self->value = data["value"];
        }

        // extract arguments
        // call instruction args get put in the var_args array
        int *p;
        int i;
        switch (self->op) {
        case (BRIL_CALL):
            if (data.contains("args") && !data["args"].empty()) {
                self->num_args = data["args"].size();
                self->arg0 = curr_program->var_args.size();
                for (json arg : data["args"]) {
                    curr_program->var_args.push_back(
                        curr_program->stringtable.lookup(arg));
                }
            }
            if (data["funcs"].size() > 1) {
                std::cerr << "Error: more than 1 func in call??\n";
                exit(1);
            }
            for (json f : data["funcs"]) {
                self->func = curr_program->stringtable.lookup(f);
            }
            break;
        case (BRIL_BR):
            self->num_args = 3;
            p = &self->arg0;
            i = 0;
            if (data["args"].size() > 1) {
                std::cerr << "Error: more than 1 arg in br??\n";
                exit(1);
            }
            for (json cond_var : data["args"]) {
                p[i++] = curr_program->stringtable.lookup(cond_var);
            }
            if (data["labels"].size() > 2) {
                std::cerr << "Error: more than 2 labels in br??\n";
                exit(1);
            }
            for (json branch : data["labels"]) {
                p[i++] = curr_program->stringtable.lookup(branch);
            }
            break;
        case (BRIL_JMP):
            self->num_args = 1;
            if (data["labels"].size() > 1) {
                std::cerr << "Error: more than 1 label in jmp??\n";
                exit(1);
            }
            for (json branch : data["labels"]) {
                self->arg0 = curr_program->stringtable.lookup(branch);
            }
            break;
        default:
            p = &self->arg0;
            i = 0;
            if (data["args"].size() > 2) {
                std::cerr << "Error: more than 2 args in operation??\n";
                exit(1);
            }
            for (json arg : data["args"]) {
                p[i++] = curr_program->stringtable.lookup(arg);
                self->num_args++;
            }
            break;
        };
        break;
    };
    return 0;
}

// make a vector<json> out of a functions args
std::vector<json> serializeFuncArgs(int start, int n) {
    std::vector<json> result;
    result.reserve(n);
    for (int i = 0; i < n; i++) {
        result.push_back(curr_program->objects[start + i].dump2json());
    }
    return result;
}

// make a vector<string> out of a calls var_args
std::vector<json> serializeInstrVargs(int start, int n) {
    std::vector<json> result;
    result.reserve(n);
    for (int i = 0; i < n; i++) {
        result.push_back(curr_program->stringtable.getString(
            curr_program->var_args[start + i]));
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
        result["name"] = curr_program->stringtable.getString(name);

        if (type.primitive) {
            result["type"] = type.dump2json();
        }

        if (num_args > 0)
            result["args"] = serializeFuncArgs(arg0, num_args);

        instrs_vec.reserve(num_instrs);
        for (int i = arg0 + num_args; i < arg0 + num_args + num_instrs; i++) {
            instrs_vec.push_back(curr_program->objects[i].dump2json());
        }
        result["instrs"] = instrs_vec;
        return result;
        break;
    case (BRIL_ARG):
        /*
         {"name": "<string>", "type": <Type>}
        */
        result["name"] = curr_program->stringtable.getString(name);
        result["type"] = type.dump2json();
        return result;
        break;
    case (BRIL_LABEL):
        // { "label": "<string>" }
        result["label"] = curr_program->stringtable.getString(name);
        return result;
        break;
    default:
        result["op"] = op2string(op);
        if (type.primitive != BRIL_VOID) {
            result["type"] = type.dump2json();
        }
        if (dest)
            result["dest"] = curr_program->stringtable.getString(dest);
        if (op == BRIL_CONST) {
            if (type.primitive == BRIL_BOOL)
                result["value"] = (bool)value;
            else
                result["value"] = value;
        }
        std::vector<json> args;
        std::vector<json> labels;
        std::vector<json> funcs;
        int *p;
        switch (op) {
        case (BRIL_CALL):
            if (num_args) {
                result["args"] = serializeInstrVargs(arg0, num_args);
            }
            funcs.push_back(curr_program->stringtable.getString(func));
            result["funcs"] = funcs;
            break;
        case (BRIL_BR):
            args.push_back(curr_program->stringtable.getString(arg0));
            labels.push_back(curr_program->stringtable.getString(arg1));
            labels.push_back(curr_program->stringtable.getString(arg2));
            result["args"] = args;
            result["labels"] = labels;
            break;
        case (BRIL_JMP):
            labels.push_back(curr_program->stringtable.getString(arg0));
            result["labels"] = labels;
            break;
        default:
            p = &arg0;
            for (int i = 0; i < num_args; i++) {
                args.push_back(curr_program->stringtable.getString(p[i]));
            }
            if (num_args > 0) {
                result["args"] = args;
            }
            break;
        };
        return result;
        break;
    };
}

int BrilObject::width() {
    if (op == BRIL_FUNC) {
        return num_args + num_instrs + 1;
    } else {
        return 1;
    }
}

bool BrilObject::isterminator() {
    return (op == BRIL_JMP || op == BRIL_BR || op == BRIL_RET);
}
bool BrilObject::islabel() { return op == BRIL_LABEL; }
bool BrilObject::isfunc() { return op == BRIL_FUNC; }

void BrilObject::print() {
    std::cout << "op: " << op2string(op) << '\n';
    std::cout << "type: " << "{" << type.primitive << ", " << type.indirection
              << "}" << '\n';
    std::cout << "name: " << curr_program->stringtable.getString(name) << '\n';
    std::cout << "num_args: " << num_args << '\n';
    std::cout << "arg0: " << arg0 << '\n';
    std::cout << "arg1: " << arg1 << '\n';
    std::cout << "arg2: " << arg2 << '\n';
    std::cout << "num_instrs: " << num_instrs << '\n';
    std::cout << '\n';
}
