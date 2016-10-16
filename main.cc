#include <iostream>
#include <string>

#include "bytecode/parser.h"
#include "bytecode/class_file.h"

int main() {
  bytecode::parser p;

  char buffer[1024];
  auto inbuf = std::cin.rdbuf();
  while (auto sz = inbuf->sgetn(buffer, 1024)) {
    p.write(buffer, sz);
  }

  auto object = p.parse();
  std::cout << object->str() << std::endl;
}
