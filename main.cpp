#include <iostream>
#include "solve.h"

int main() {
  std::string s, c;
  size_t len;
  std::cin >> s >> c >> len;
  std::cout << solve(s, c, len) << std::endl;
}

///   ab+c.aba.*.bac.+.+* b 2
///   INF
///   acb..bab.c.*.ab.ba.+.+*a. a 2
///   4
