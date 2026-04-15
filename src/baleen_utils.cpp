#include <baleen_utils.h>
#include <iostream>

int uniqueRename(BrilProgram &p, int name) {
    const std::string &original = p.stringtable.getString(name);
    int i = 0;
    std::string newstring = original + "_";
    while (i < 10000) {
        std::string numstring = newstring + std::to_string(i);
        if (!p.stringtable.contains(numstring)) {
            return p.stringtable.lookup(numstring);
        }
        i++;
    }
    std::cout << "Error: unique rename failed\n";
    exit(1);
    return -1;
}
