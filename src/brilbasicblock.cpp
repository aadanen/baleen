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
    int index = start;
    for (int i = 0; i < length; i++) {
        curr_program->objects[index].print();
        index += curr_program->objects[index].offset();
    }
    std::cout << "===\t\t\t===\n";
}

