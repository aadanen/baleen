#include <brilfunction.h>

BrilFunction::BrilFunction() : idx(0), entry(0), length(0) { flags = 0; }
BrilFunction::BrilFunction(int idx, int entry, int length)
    : idx(idx), entry(entry), length(length) {
    flags = 0;
}

