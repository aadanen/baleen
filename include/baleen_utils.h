#ifndef BALEEN_UTILS_H_
#define BALEEN_UTILS_H_

#include <brilprogram.h>

// given a stringtable and one of its entries, generate and insert a new
// unique string based on the old entry
// if the original variable was var1, it does a linear scan over
// var1_0, var1_1, var1_2... until it finds a new string
int uniqueRename(BrilProgram &p, int name);
#endif /* BALEEN_UTILS_H_ */
