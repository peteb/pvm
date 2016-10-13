#include <iostream>
#include <string>

#include "bytecode/parser.h"

int main() {
  std::cout << "Hello" << std::endl;

  char data[] = "HEJ";
  char data2[] = "KAT\1";
  bytecode::parser p;
  p.parse(data, 3);
  p.parse(data2, 5);
  p.parse(data, 3);
  p.parse(data, 3);
  p.parse(data, 3);
  p.parse(data, 3);
  p.parse(data, 3);
  p.parse(data, 3);

  std::cout << "Instructions:" << std::endl;

  for (const auto &instruction : p.consume_instructions()) {
    std::cout << std::string(instruction) << std::endl;
  }
}
