#include <brilprogram.h>
#include <iostream>

int main() {
    json data = json::parse(stdin);
    BrilProgram p(data);
    p.optimize();
    json out = p.dump2json();
    std::cout << out.dump(2) << '\n';
    return 0;
}
