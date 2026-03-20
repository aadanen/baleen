#include <brilbasicblock.h>
#include <brilprogram.h>
#include <iostream>

BrilBasicBlock::BrilBasicBlock() : name(0), start(0), length(0) {}
BrilBasicBlock::BrilBasicBlock(int name, int start, int length)
    : name(name), start(start), length(length) {
    flags = 0;
}

void BrilBasicBlock::print() {
    std::cout << "===\t" << curr_program->stringtable.getString(name)
              << "\t===\n";
    for (int i = start; i < start + length; i++) {
        curr_program->objects[i].print();
    }
    std::cout << "===\t\t\t===\n";
}

