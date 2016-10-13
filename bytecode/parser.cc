#include "parser.h"

#include <iostream>
#include <iomanip>

using bytecode::instruction;

void bytecode::parser::parse(const char *data, size_t size) {
  buffer.insert(buffer.end(), data, data + size);
}

std::vector<instruction> bytecode::parser::consume_instructions() {
  return {};
}

void bytecode::parser::dump_buffer() {
  int pos = 0;
  auto format(std::cout.flags());

  for (char byte : buffer) {
    std::cout << std::setfill('0') << std::setw(2) << std::hex << +byte;

    if (pos == 15) {
      std::cout << std::endl;
    }
    else if (pos % 4 == 3) {
      std::cout << "  ";
    }
    else {
      std::cout << " ";
    }

    ++pos;
  }

  std::cout.flags(format);
}
