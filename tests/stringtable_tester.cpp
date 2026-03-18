#include <iostream>
#include <stringtable.h>

int st_simple() {
  StringTable st;
  int loc = st.lookup(std::string("hello world"));
  std::string result = st.getString(loc);
  if (result == "hello world") {
    return 0;
  } else {
    return 1;
  }
}

int st_simple_reinsert() {
  StringTable st;
  int loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  std::string result = st.getString(loc);
  if (result == "hello world") {
    return 0;
  } else {
    return 1;
  }
}

int st_simple_multi_insert() {
  StringTable st;
  int loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("foo"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("baz"));
  loc = st.lookup(std::string("hello world"));
  int barloc = st.lookup(std::string("bar"));
  loc = st.lookup(std::string("hello world"));
  loc = st.lookup(std::string("buzz"));
  if (loc == 0)
    return 1;
  std::string result = st.getString(barloc);
  if (result == "bar") {
    return 0;
  } else {
    return 1;
  }
}

int test_stringtable() {
  int ret = 0;
  ret = st_simple();
  if (ret) {
    std::cerr << "st_simple() failed\n";
    return ret;
  }

  ret = st_simple_reinsert();
  if (ret) {
    std::cerr << "st_simple_reinsert() failed\n";
    return ret;
  }

  ret = st_simple_multi_insert();
  if (ret) {
    std::cerr << "st_simple_multi_insert() failed\n";
    return ret;
  }
  return 0;
}

int main(int argc, char **argv) { return test_stringtable(); }
