#ifndef BRIL_FUNCTION_H_
#define BRIL_FUNCTION_H_

struct BrilFunction {
    int idx;    // index of the function in the objects[] array
    int entry;  // index of the brilbasicblock at the start of the func
    int length; // number of basic blocks in the function
    int flags;
    BrilFunction();
    BrilFunction(int func_idx, int entry, int length);
};

#endif /* BRIL_FUNCTION_H_ */
