#include <brilobject.h>
#include <brilprogram.h>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  json data;
  if (argc == 1) {
    data = json::parse(stdin);
  } else {
    std::ifstream f(argv[1]);
    if (!f) {
      std::cerr << "error: failed to open file\n";
      exit(1);
    }
    data = json::parse(f);
  }
  BrilProgram p(data);
  json out = p.dump2json();
  std::cout << out.dump(2) << '\n';
  if (data != out) {
    std::cerr << argv[1] << ": json round trip failed\n";
    return 1;
  }
  return 0;
}
