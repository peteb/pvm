// -*- c++ -*-

#ifndef _BYTECODE_PARSER_H
#define _BYTECODE_PARSER_H

#include <cstddef>
#include <vector>

#include "instruction.h"

namespace bytecode {
  class parser {
  public:
    /**
     * Parses so many instructions it can. Saves the leftover data
     * for the next invocation.
     */
    void parse(const char *data, size_t size);

    /**
     * Fetches all the parsed instructions.
     */
    std::vector<instruction> consume_instructions();

  private:
    void dump_buffer();

    std::vector<char> buffer;
  };

}

#endif // !_BYTECODE_PARSER_H
