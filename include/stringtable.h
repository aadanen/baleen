#ifndef STRING_TABLE_H_
#define STRING_TABLE_H_

#include <map>
#include <string>
#include <vector>

class StringTable {
  private:
    std::vector<std::string> strings;
    std::map<std::string, int> entries;

  public:
    const std::string &getString(int);
    int lookup(const std::string &str);
};

#endif /* STRING_TABLE_H_ */
