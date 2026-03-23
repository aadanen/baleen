#include <brilobject.h>
#include <brilprogram.h>
#include <fstream>
#include <iostream>
#include <cstdlib>

int test_serde(json data) {
  BrilProgram p(data);
  json out = p.dump2json();
  std::cout << out.dump(2) << '\n';
  if (data != out) {
    std::cerr << "Error: json round trip failed\n";
    return 1;
  }
  return 0;
}

int test_interp(json data, char *path_to_infile) {
  BrilProgram a(data);
  //a.eliminateDeadCode();

  std::string ifname = std::string(path_to_infile);
  std::string ofname = ifname + std::string(".opt");

  // dump to temporary file
  std::ofstream outfile(ofname);
  if (!outfile) {
      std::cout << "Error: failed to open temporary file\n";
      return 1;
  }
  std::string out = a.dump2json().dump(2);
  std::cout << out << '\n';
  outfile << out << '\n';
  outfile.flush();

  std::string o_cmd = std::string("brili < " + ofname + "\n");
  std::string i_cmd = std::string("brili < " + ifname + "\n");

  int a_result = std::system(o_cmd.c_str());
  int b_result = std::system(i_cmd.c_str());
  if (a_result != b_result)
    return 1;
  else
    return 0;
}

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

  if (test_serde(data))
    return 1;
  if (argc > 1 && test_interp(data, argv[1]))
    return 1;
  return 0;
}
