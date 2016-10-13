// -*- c++ -*-

#ifndef _BYTECODE_PARSER_H
#define _BYTECODE_PARSER_H

#include <cstddef>
#include <vector>
#include <functional>
#include <memory>

#include "instruction.h"

namespace bytecode {
  class class_file;

  class parser {
  public:
    parser();

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

    template<typename T>
    T *peek() {
      return reinterpret_cast<T *>(buffer.data() + cursor);
    }

    template<typename T>
    T *read() {
      if (cursor + sizeof(T) >= buffer.size()) {
        return nullptr;
      }

      T *ret = peek<T>();
      cursor += sizeof(T);
      return ret;
    }

    bool try_parse();
    bool parse_header();
    bool parse_cp_item();

    std::function<bool(parser *)> state;
    std::vector<char> buffer;
    std::ptrdiff_t cursor;
    std::shared_ptr<class_file> object;
  };

}

#endif // !_BYTECODE_PARSER_H
