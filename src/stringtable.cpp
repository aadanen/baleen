#include <stringtable.h>

StringTable::StringTable() { this->strings.push_back("dummy"); }

// O(1) lookup
const std::string &StringTable::getString(int i) {
    if (i < 0 || i > (int)this->strings.size()) {
        fprintf(stderr, "invalid stringtable access\n");
        exit(1);
    }
    return this->strings[i];
}

// O(log n) lookup/insertion
int StringTable::lookup(const std::string &str) {
    auto item = this->entries.find(str);
    if (item == entries.end()) {
        this->strings.push_back(std::string(str));
        entries.insert({str, strings.size() - 1});
        return strings.size() - 1;
    } else {
        return item->second;
    }
}
