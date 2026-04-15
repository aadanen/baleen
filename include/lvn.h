#ifndef LVN_H_
#define LVN_H_

#include <brilobject.h>
#include <map>
#include <unordered_map>

class LVN {
private:
  // instr2num and num2instr implement a bimap for fast lookups

  // map from instructions to their value number
  // the brilobjects here are copies of the original instr in objects[]
  std::unordered_map<BrilObject, int, BrilObjectHasher> instr2num;

  // map from value numbers to a instruction (new instr, not in objects[])
  // the brilobjects here may be modified, and not actually appear in the
  // original program. For example, they might be renamed to have a unique
  // name.
  std::map<int, BrilObject> num2instr;

  // map from variable names to their value number
  // the lvn "environment" that changes over time
  std::map<int, int> var2num;

public:
  bool contains(const BrilObject &instr) const;
  bool contains(int name) const;
  int insert(BrilObject instr);
  void updateEnv(int name, int vn);
  int lookupVN(BrilObject instr) const;
  int lookupVN(int name) const;
  const BrilObject *lookupInstr(int vn) const;
};

#endif /* LVN_H_ */
