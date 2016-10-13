#include "parser.h"

#include <iostream>
#include <iomanip>

#include "class_file.h"

using bytecode::instruction;
using bytecode::class_file;

bytecode::parser::parser() {
  state = &parser::parse_header;
  cursor = 0;
}

void bytecode::parser::parse(const char *data, size_t size) {
  buffer.insert(buffer.end(), data, data + size);
  while (try_parse());
}

bool bytecode::parser::try_parse() {
  if (buffer.empty()) {
    return false;
  }

  const ptrdiff_t start = cursor;
  if (!state(this)) {
    // rewind
    cursor = start;
    return false;
  }

  return true;
}

bool bytecode::parser::parse_header() {
  if (auto *hdr = read<header_t>()) {
    // TODO: verify that the header looks OK
    object = std::make_shared<class_file>();

    return true;
  }
  else {
    return false;
  }
}

bool bytecode::parser::parse_cp_item() {
  return false;
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
