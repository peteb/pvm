// -*- c++ -*-

#ifndef _BYTECODE_PARSER_H
#define _BYTECODE_PARSER_H

#include <cstddef>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>

#include "class_file.h"

namespace bytecode {
  class class_file;

  class parser {
  public:
    void write(const char *data, std::size_t size);
    std::shared_ptr<class_file> parse();

  private:
    void dump_buffer();

    template<typename T>
    T *read(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes > buffer.size()) {
        throw std::runtime_error("not enough bytes");
      }

      T *ret = reinterpret_cast<T *>(buffer.data() + cursor);
      cursor += bytes;
      return ret;
    }

    template<typename T>
    T *peek(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes > buffer.size()) {
        return nullptr;
      }

      return reinterpret_cast<T *>(buffer.data() + cursor);
    }

    void header();
    void constant_pool();
    bytecode::cp_info_t *constant_pool_item();
    void interfaces();
    void fields();
    void methods();
    bytecode::field field();
    void attributes();
    bytecode::attribute_info_t *attribute();

    std::vector<char> buffer;
    std::ptrdiff_t cursor = 0;
    std::shared_ptr<class_file> object;
  };

}

#endif // !_BYTECODE_PARSER_H
