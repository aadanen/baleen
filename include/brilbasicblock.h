#ifndef BRIL_BASIC_BLOCK_H_
#define BRIL_BASIC_BLOCK_H_

enum BrilBlockFlags {
    IS_ALIVE = 0x1,
};

struct BrilBasicBlock {
    int name;   // index into the stringtable
    int start;  // index of first instruction in objects
    int length; // index of the number of instructions in the block
    int flags;
    BrilBasicBlock();
    BrilBasicBlock(int name, int start, int length);
    void print();
};

// basic blocks always start immediately after a function declaration or label

#endif /* BRIL_BASIC_BLOCK_H_ */
