#include <brilobject.h>
#include <brilprogram.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

int test_serde(json data) {
  BrilProgram p(data);
  json out = p.dump2json();
  // std::cout << out.dump(2) << '\n';
  if (data != out) {
    std::cerr << "Error: json round trip failed\n";
    return 1;
  }
  return 0;
}

int test_interp(json data, char *path_to_infile) {
  BrilProgram a(data);
  a.optimize();

  std::string ifname = std::string(path_to_infile);
  std::string ofname = ifname + std::string(".opt");

  int start = ifname.rfind("/");
  int end = ifname.find(".", start);
  std::string testname = ifname.substr(start + 1, end - start - 1);

  std::string optresult = ifname.substr(0, start + 1) + testname + "_opt.txt";
  std::string ogresult = ifname.substr(0, start + 1) + testname + ".txt";

  // dump to temporary file
  std::ofstream outfile(ofname);
  if (!outfile) {
    std::cout << "Error: failed to open temporary file\n";
    return 1;
  }
  std::string out = a.dump2json().dump(2);
  // std::cout << out << '\n';
  outfile << out << '\n';
  outfile.flush();

  std::string o_cmd = std::string("brili < " + ofname + " > " + optresult);
  std::string i_cmd = std::string("brili < " + ifname + " > " + ogresult);

  int a_result = std::system(o_cmd.c_str());
  int b_result = std::system(i_cmd.c_str());

  std::ifstream o_stm(optresult);
  std::ifstream i_stm(ogresult);
  char o_buf;
  char i_buf;
  bool matching = true;
  while (matching) {
    o_stm >> o_buf;
    i_stm >> i_buf;
    if (o_buf != i_buf) {
      matching = false;
      break;
    }
    if (o_stm.eof() || i_stm.eof())
      break;
  }

  if (o_stm.eof() + i_stm.eof() == 1)
    matching = false;

  if ((a_result != b_result) || !matching)
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
