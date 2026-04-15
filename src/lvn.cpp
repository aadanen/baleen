#include <lvn.h>
bool LVN::contains(const BrilObject &instr) const {
    return instr2num.find(instr) != instr2num.end();
}
bool LVN::contains(int name) const {
    return num2instr.find(name) != num2instr.end();
}
int LVN::insert(BrilObject instr) {
    if (contains(instr))
        exit(1);
    int vn = instr2num.size();
    instr2num[instr] = vn;
    num2instr[vn] = instr;
    return vn;
}
int LVN::lookupVN(BrilObject instr) const {
    auto it = instr2num.find(instr);
    if (it == instr2num.end())
        return -1;
    else
        return it->second;
}
int LVN::lookupVN(int name) const {
    auto it = var2num.find(name);
    if (it == var2num.end())
        return -1;
    else
        return it->second;
}

// raw pointer is fine because num2instr is a std::set and those are stable
const BrilObject *LVN::lookupInstr(int vn) const {
    auto it = num2instr.find(vn);
    if (it == num2instr.end())
        return nullptr;
    else
        return &(it->second);
}

void LVN::updateEnv(int name, int vn) { var2num[name] = vn; }
