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

    std::shared_ptr<class_file> release();

  private:
    void dump_buffer();

    template<typename T>
    T *peek(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes >= buffer.size()) {
        return nullptr;
      }

      return reinterpret_cast<T *>(buffer.data() + cursor);
    }

    template<typename T>
    T *read(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes >= buffer.size()) {
        return nullptr;
      }

      T *ret = reinterpret_cast<T *>(buffer.data() + cursor);
      cursor += bytes;
      return ret;
    }

    bool try_parse();
    bool parse_header();
    bool parse_cp();
    bool parse_cp_item();
    bool parse_midriff();
    bool parse_if();
    bool parse_fields();
    bool parse_field_item();
    bool parse_methods();
    bool parse_method_items();
    bool parse_attributes();
    bool parse_attribute_items();

    std::function<bool(parser *)> state;
    std::vector<char> buffer;
    std::ptrdiff_t cursor = 0;
    std::shared_ptr<class_file> object;
    std::size_t cp_items_left = 0;
    std::size_t field_items_left = 0;
    std::size_t method_items_left = 0;
    std::size_t attribute_items_left = 0;
  };

}

#endif // !_BYTECODE_PARSER_H
